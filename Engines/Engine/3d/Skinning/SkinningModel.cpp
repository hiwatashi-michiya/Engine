#include "SkinningModel.h"
#include <cassert>
#include "Engine/Convert.h"
#include "Engine/Drawing/ShaderManager.h"
#include <fstream>
#include <sstream>
#include "Drawing/ImGuiManager.h"
#include "Drawing/RootSignatureManager.h"
#include "Buffer/BufferResource.h"
#include "Drawing/PipelineManager.h"
#include "Drawing/MeshManager.h"
#include "Core/DirectXSetter.h"
#include "Drawing/RenderManager.h"

#pragma comment(lib, "dxcompiler.lib")

ID3D12Device* SkinningModel::device_ = nullptr;
ID3D12GraphicsCommandList* SkinningModel::commandList_ = nullptr;
ID3D12RootSignature* SkinningModel::rootSignature_ = nullptr;
ID3D12PipelineState* SkinningModel::pipelineStates_[SkinningModel::BlendMode::kCountBlend] = { nullptr };
//ID3D12PipelineState* Model::pipelineState_ = nullptr;
IDxcBlob* SkinningModel::vs3dBlob_ = nullptr;
IDxcBlob* SkinningModel::ps3dBlob_ = nullptr;
int SkinningModel::modelNumber_ = 0;
SkinningModel::BlendMode SkinningModel::currentBlendMode_ = SkinningModel::BlendMode::kNormal;
const char* SkinningModel::BlendTexts[SkinningModel::BlendMode::kCountBlend] = { "Normal", "Add", "Subtract", "Multiply", "Screen" };

void SkinningModel::StaticInitialize(ID3D12Device* device) {

	assert(device);

	HRESULT hr;

	device_ = device;

	//Shaderをコンパイルする
	vs3dBlob_ = ShaderManager::GetInstance()->CompileShader(L"./Resources/shaders/SkinningObject3d.VS.hlsl", ShaderManager::kVS, "VSSkinning3D");

	ps3dBlob_ = ShaderManager::GetInstance()->CompileShader(L"./Resources/shaders/Object3d.PS.hlsl", ShaderManager::kPS, "PS3D");

	//頂点レイアウト
	std::array<D3D12_INPUT_ELEMENT_DESC, 5> inputElementDescs = {};
	inputElementDescs[0].SemanticName = "POSITION";
	inputElementDescs[0].SemanticIndex = 0;
	inputElementDescs[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	inputElementDescs[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	inputElementDescs[1].SemanticName = "TEXCOORD";
	inputElementDescs[1].SemanticIndex = 0;
	inputElementDescs[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	inputElementDescs[1].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	inputElementDescs[2].SemanticName = "NORMAL";
	inputElementDescs[2].SemanticIndex = 0;
	inputElementDescs[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	inputElementDescs[2].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	inputElementDescs[3].SemanticName = "WEIGHT";
	inputElementDescs[3].SemanticIndex = 0;
	inputElementDescs[3].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	inputElementDescs[3].InputSlot = 1; //一番目のslotのVBVを使用
	inputElementDescs[3].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	inputElementDescs[4].SemanticName = "INDEX";
	inputElementDescs[4].SemanticIndex = 0;
	inputElementDescs[4].Format = DXGI_FORMAT_R32G32B32A32_SINT;
	inputElementDescs[4].InputSlot = 1; //一番目のslotのVBVを使用
	inputElementDescs[4].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

	D3D12_INPUT_LAYOUT_DESC inputLayoutDesc{};
	inputLayoutDesc.pInputElementDescs = inputElementDescs.data();
	inputLayoutDesc.NumElements = UINT(inputElementDescs.size());

	//RootSignature作成
	D3D12_ROOT_SIGNATURE_DESC descriptionRootSignature{};
	descriptionRootSignature.Flags =
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	//D3D12_ROOT_PARAMETER rootParameters[1];
	//rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	//rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	//rootParameters[0].Descriptor.ShaderRegister = 0;
	//descriptionRootSignature.pParameters = rootParameters; //ルートパラメータ配列へのポインタ
	//descriptionRootSignature.NumParameters = 1; //ルートパラメータの長さ

	D3D12_DESCRIPTOR_RANGE descriptorRange[1] = {};
	descriptorRange[0].BaseShaderRegister = 0;
	descriptorRange[0].NumDescriptors = 1;
	descriptorRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	descriptorRange[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	D3D12_DESCRIPTOR_RANGE descriptorRangeMask[1] = {};
	descriptorRangeMask[0].BaseShaderRegister = 1;
	descriptorRangeMask[0].NumDescriptors = 1;
	descriptorRangeMask[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	descriptorRangeMask[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	D3D12_DESCRIPTOR_RANGE descriptorRangeForMatrixPalette[1] = {};
	descriptorRangeForMatrixPalette[0].BaseShaderRegister = 0;
	descriptorRangeForMatrixPalette[0].NumDescriptors = 1;
	descriptorRangeForMatrixPalette[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	descriptorRangeForMatrixPalette[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	//ルートパラメータ作成
	D3D12_ROOT_PARAMETER rootParameters[8]{};
	rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	rootParameters[0].Descriptor.ShaderRegister = 0;

	rootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	rootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
	rootParameters[1].Descriptor.ShaderRegister = 0;

	rootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE; //DescriptorTableを使う
	rootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL; //PixelShaderで使う
	rootParameters[2].DescriptorTable.pDescriptorRanges = descriptorRange; //Tableの中身の配列を指定
	rootParameters[2].DescriptorTable.NumDescriptorRanges = _countof(descriptorRange); //Tableで利用する数

	//平行光源
	rootParameters[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV; //CBVを使う
	rootParameters[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL; //PixelShaderで使う
	rootParameters[3].Descriptor.ShaderRegister = 1; //レジスタ番号1を使う

	//カメラの位置を送る
	rootParameters[4].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	rootParameters[4].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	rootParameters[4].Descriptor.ShaderRegister = 2;

	//点光源
	rootParameters[5].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	rootParameters[5].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	rootParameters[5].Descriptor.ShaderRegister = 3;

	//MatrixPalette
	rootParameters[6].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE; //DescriptorTableを使う
	rootParameters[6].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
	rootParameters[6].DescriptorTable.pDescriptorRanges = descriptorRangeForMatrixPalette; //Tableの中身の配列を指定
	rootParameters[6].DescriptorTable.NumDescriptorRanges = _countof(descriptorRangeForMatrixPalette); //Tableで利用する数

	//MatrixPalette
	rootParameters[7].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE; //DescriptorTableを使う
	rootParameters[7].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	rootParameters[7].DescriptorTable.pDescriptorRanges = descriptorRangeMask; //Tableの中身の配列を指定
	rootParameters[7].DescriptorTable.NumDescriptorRanges = _countof(descriptorRangeMask); //Tableで利用する数

	descriptionRootSignature.pParameters = rootParameters; //ルートパラメータ配列へのポインタ
	descriptionRootSignature.NumParameters = _countof(rootParameters); //ルートパラメータの長さ

	//Samplerの設定
	D3D12_STATIC_SAMPLER_DESC staticSamplers[1] = {};
	staticSamplers[0].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR; //バイリニアフィルタ
	staticSamplers[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP; //0～1の範囲外をリピート
	staticSamplers[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER; //比較しない
	staticSamplers[0].MaxLOD = D3D12_FLOAT32_MAX; //ありったけのMipmapを使う
	staticSamplers[0].ShaderRegister = 0; //レジスタ番号0を使う
	staticSamplers[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL; //PixelShaderで使う
	descriptionRootSignature.pStaticSamplers = staticSamplers;
	descriptionRootSignature.NumStaticSamplers = _countof(staticSamplers);

	//シリアライズしてバイナリにする
	Microsoft::WRL::ComPtr<ID3DBlob> signatureBlob = nullptr;
	Microsoft::WRL::ComPtr<ID3DBlob> errorBlob = nullptr;
	hr = D3D12SerializeRootSignature(&descriptionRootSignature,
		D3D_ROOT_SIGNATURE_VERSION_1, &signatureBlob, &errorBlob);
	if (FAILED(hr)) {
		Log(reinterpret_cast<char*>(errorBlob->GetBufferPointer()));
		assert(false);
	}

	RootSignatureManager::GetInstance()->CreateRootSignature(signatureBlob, "RootSignature3DSkinning");

	rootSignature_ = RootSignatureManager::GetInstance()->GetRootSignature("RootSignature3DSkinning");

	//Blendstateの設定
	D3D12_BLEND_DESC blendDesc{};
	//全ての色要素を書き込む
	blendDesc.RenderTarget[0].RenderTargetWriteMask =
		D3D12_COLOR_WRITE_ENABLE_ALL;
	blendDesc.RenderTarget[0].BlendEnable = TRUE;
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;

	//RasterizerStateの設定
	D3D12_RASTERIZER_DESC rasterizerDesc{};
	//裏面(時計回り)を表示しない
	rasterizerDesc.CullMode = D3D12_CULL_MODE_BACK;
	//三角形の中を塗りつぶす
	rasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;

	D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsPipelineStateDesc{};
	graphicsPipelineStateDesc.pRootSignature = rootSignature_; //RootSignature
	graphicsPipelineStateDesc.InputLayout = inputLayoutDesc; //InputLayout
	graphicsPipelineStateDesc.VS = { vs3dBlob_->GetBufferPointer(),
	vs3dBlob_->GetBufferSize() }; //VertexShader
	graphicsPipelineStateDesc.PS = { ps3dBlob_->GetBufferPointer(),
	ps3dBlob_->GetBufferSize() }; //PixelShader
	graphicsPipelineStateDesc.RasterizerState = rasterizerDesc; //RasterizerState
	//書き込むRTVの情報
	graphicsPipelineStateDesc.NumRenderTargets = 1;
	graphicsPipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	//利用するトポロジ(形状)のタイプ、三角形
	graphicsPipelineStateDesc.PrimitiveTopologyType =
		D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	//どのように画面に色を打ち込むかの設定(気にしなくて良い)
	graphicsPipelineStateDesc.SampleDesc.Count = 1;
	graphicsPipelineStateDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;
	//DepthStencilStateの設定
	D3D12_DEPTH_STENCIL_DESC depthStencilDesc{};
	//Depthの機能を有効化する
	depthStencilDesc.DepthEnable = true;
	//書き込み
	depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	//近ければ描画
	depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;

	//DepthStencilの設定
	graphicsPipelineStateDesc.DepthStencilState = depthStencilDesc;
	graphicsPipelineStateDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;

	//通常
	blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
	graphicsPipelineStateDesc.BlendState = blendDesc; //BlendState

	PipelineManager::GetInstance()->CreatePipeLine(graphicsPipelineStateDesc, "PipelineNormal3DSkinning");

	pipelineStates_[kNormal] = PipelineManager::GetInstance()->GetPipeline("PipelineNormal3DSkinning");

	//加算
	blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_ONE;
	graphicsPipelineStateDesc.BlendState = blendDesc; //BlendState

	PipelineManager::GetInstance()->CreatePipeLine(graphicsPipelineStateDesc, "PipelineAdd3DSkinning");

	pipelineStates_[kAdd] = PipelineManager::GetInstance()->GetPipeline("PipelineAdd3DSkinning");

	//減算
	blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_SUBTRACT;
	blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_ONE;
	graphicsPipelineStateDesc.BlendState = blendDesc; //BlendState

	PipelineManager::GetInstance()->CreatePipeLine(graphicsPipelineStateDesc, "PipelineSubtract3DSkinning");

	pipelineStates_[kSubtract] = PipelineManager::GetInstance()->GetPipeline("PipelineSubtract3DSkinning");

	//乗算
	blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_ZERO;
	blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_SRC_COLOR;
	graphicsPipelineStateDesc.BlendState = blendDesc; //BlendState

	PipelineManager::GetInstance()->CreatePipeLine(graphicsPipelineStateDesc, "PipelineMultiply3DSkinning");

	pipelineStates_[kMultiply] = PipelineManager::GetInstance()->GetPipeline("PipelineMultiply3DSkinning");

	//スクリーン
	blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_INV_DEST_COLOR;
	blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_ONE;
	graphicsPipelineStateDesc.BlendState = blendDesc; //BlendState

	PipelineManager::GetInstance()->CreatePipeLine(graphicsPipelineStateDesc, "PipelineScreen3DSkinning");

	pipelineStates_[kScreen] = PipelineManager::GetInstance()->GetPipeline("PipelineScreen3DSkinning");

	currentBlendMode_ = BlendMode::kNormal;

	//メッシュの静的初期化
	Mesh::StaticInitialize(device);
	//マテリアルの静的初期化
	Material::StaticInitialize(device);

}

SkinningModel* SkinningModel::Create(const std::string& filename, int32_t number) {

	SkinningModel* model = new SkinningModel();

	model->Initialize(filename, number);

	return model;

}

void SkinningModel::Initialize(const std::string& filename, int32_t number) {

	assert(device_);

	if (MeshManager::GetInstance()->IsExistMesh(filename)) {

		mesh_ = MeshManager::GetInstance()->GetMesh(filename);

	}
	else {

		//メッシュを登録
		MeshManager::GetInstance()->CreateMesh(filename);
		mesh_ = MeshManager::GetInstance()->GetMesh(filename);

	}

	material_ = std::make_unique<Material>();
	material_->Create();

	meshFilePath_ = filename;

	texture_ = TextureManager::GetInstance()->Load(mesh_->textureFilePath_);
	maskTexture_ = TextureManager::GetInstance()->Load("./Resources/EngineResources/defaultMask.png");

	//skeleton,skinCluster設定
	{

		LoadAnimation(filename, number);

		skeleton_ = std::make_unique<Skeleton>();

		*skeleton_ = CreateSkeleton(mesh_->modelData_.rootNode);

		skinCluster_ = std::make_unique<SkinCluster>();

		skinCluster_->Create(device_, *skeleton_, mesh_->modelData_);

		currentFileName_ = stringMap_[number];

		//最初に表示するように初期化
		ApplyAnimation(*skeleton_, *animations_[currentFileName_], animationTime_);
		UpdateSkeleton(*skeleton_);
		skinCluster_->Update(*skeleton_);

	}

	//transformMatrix
	{

		matBuff_ = CreateBufferResource(device_, sizeof(TransformationMatrix));

		matBuff_->SetName(L"matrixBuff");

		matBuff_->Map(0, nullptr, reinterpret_cast<void**>(&matTransformMap_));

		matTransformMap_->WVP = MakeIdentity4x4();
		matTransformMap_->World = MakeIdentity4x4();

		matBuff_->Unmap(0, nullptr);

	}

	//カメラ設定
	{

		cameraBuff_ = CreateBufferResource(device_, sizeof(CameraForGPU));

		cameraBuff_->SetName(L"cameraBuff");

		cameraBuff_->Map(0, nullptr, reinterpret_cast<void**>(&cameraMap_));

		cameraMap_->worldPosition = Vector3::Zero();

		cameraBuff_->Unmap(0, nullptr);

	}

	localMatrix_ = MakeIdentity4x4();
	worldMatrix_ = Matrix4x4::Identity();

}

void SkinningModel::LoadAnimation(const std::string& filename, int32_t number) {

	//ナンバーが無かったら新規生成
	if (stringMap_.find(number) == stringMap_.end()) {
		stringMap_[number] = filename;
	}

	//既に生成されていたら処理中断
	if (animations_.find(stringMap_[number]) != animations_.end()) {
		return;
	}

	animations_[stringMap_[number]] = std::make_unique<Animation>();

	*animations_[stringMap_[number]] = LoadAnimationFile(stringMap_[number]);

}


void SkinningModel::SetAnimation(int32_t number, bool isReset) {

	if (stringMap_.find(number) == stringMap_.end()) {

		MessageBox(NULL, L"指定した'number'に対応したファイル名は存在しません。", L"SkinningModel::SetAnimation", MB_OK);

		return;

	}

	currentFileName_ = stringMap_[number];

	if (isReset) {
		ResetAnimation();
	}

}

void SkinningModel::ResetAnimation() {

	//値をリセットして再適用
	animationTime_ = 0.0f;
	ApplyAnimation(*skeleton_, *animations_[currentFileName_], animationTime_);

}

void SkinningModel::UpdateAnimation() {

	isEndAnimation_ = false;

	//アニメーションが存在していて、再生フラグが立っている時
	if (animations_[currentFileName_] and isStartAnimation_ and animations_[currentFileName_]->nodeAnimations.size() != 0) {

		//現在のアニメーションタイムをアニメーション速度分加算
		animationTime_ += animationSpeed_ / 60.0f;

		//アニメーションタイムが全体の尺を超えていたら終点とみなす
		if (animationTime_ >= animations_[currentFileName_]->duration) {

			animationTime_ = animations_[currentFileName_]->duration;

			//ループしなければフラグを降ろす
			if (!isLoop_) {
				isStartAnimation_ = false;
			}

			isEndAnimation_ = true;
		}

		//アニメーションの時間調整
		animationTime_ = std::fmod(animationTime_, animations_[currentFileName_]->duration);

		ApplyAnimation(*skeleton_, *animations_[currentFileName_], animationTime_);
		UpdateSkeleton(*skeleton_);

		skinCluster_->Update(*skeleton_);

		//NodeAnimation& rootNodeAnimation = animation_->nodeAnimations[mesh_->modelData_.rootNode.name]; //rootNodeのanimationを取得
		//Vector3 translate = CalculateValue(rootNodeAnimation.translate.keyFrames, animationTime_);
		//Quaternion rotate = CalculateValue(rootNodeAnimation.rotate.keyFrames, animationTime_);
		//Vector3 scale = CalculateValue(rootNodeAnimation.scale.keyFrames, animationTime_);
		//localMatrix_ = MakeAffineMatrix(scale, rotate, translate);

	}

}

void SkinningModel::PreDraw(ID3D12GraphicsCommandList* commandList) {

	assert(commandList_ == nullptr);

	commandList_ = commandList;

	//PSO設定
	commandList_->SetPipelineState(pipelineStates_[currentBlendMode_]);
	//ルートシグネチャを設定
	commandList_->SetGraphicsRootSignature(rootSignature_);
	//プリミティブ形状を設定
	commandList_->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

}

void SkinningModel::PostDraw() {

	commandList_ = nullptr;

	modelNumber_ = 0;

}

void SkinningModel::Draw(Camera* camera) {

	Matrix4x4 worldViewProjectionMatrix;

	worldViewProjectionMatrix = worldMatrix_ * camera->matViewProjection_;
	matTransformMap_->WVP = worldViewProjectionMatrix;
	matTransformMap_->World = worldMatrix_;
	matTransformMap_->WorldInverseTranspose = Transpose(Inverse(worldMatrix_));

	cameraMap_->worldPosition = camera->GetWorldPosition();

	RenderManager::GetInstance()->AddSkinningModel(this);

}

void SkinningModel::Draw(const Matrix4x4& localMatrix, Camera* camera) {

	Matrix4x4 worldViewProjectionMatrix = localMatrix * worldMatrix_ * camera->matViewProjection_;
	matTransformMap_->WVP = worldViewProjectionMatrix;
	matTransformMap_->World = localMatrix * worldMatrix_;
	matTransformMap_->WorldInverseTranspose = Transpose(Inverse(localMatrix * worldMatrix_));

	cameraMap_->worldPosition = camera->GetWorldPosition();

	RenderManager::GetInstance()->AddSkinningModel(this);

}

void SkinningModel::Render()
{

	D3D12_VERTEX_BUFFER_VIEW vbvs[2] = {
		mesh_->vbView_,
		skinCluster_->influenceBufferView_
	};

	//カメラ設定
	commandList_->SetGraphicsRootConstantBufferView(4, cameraBuff_->GetGPUVirtualAddress());

	commandList_->SetGraphicsRootConstantBufferView(1, matBuff_->GetGPUVirtualAddress());

	commandList_->SetGraphicsRootDescriptorTable(2, texture_->srvHandleGPU);
	commandList_->SetGraphicsRootDescriptorTable(7, maskTexture_->srvHandleGPU);

	commandList_->SetGraphicsRootDescriptorTable(6, skinCluster_->paletteSrvHandle_.second);

	//描画
	material_->SetCommandMaterial(commandList_);

	commandList_->IASetVertexBuffers(0, 2, vbvs);

	mesh_->SetCommandMeshForSkinning(commandList_);

}

void SkinningModel::DrawSkeleton(Camera* camera) {

	if (skeleton_) {
		DrawSkeletonLine(*skeleton_, camera, worldMatrix_);
	}

}

void SkinningModel::Finalize() {

}

//デバッグ時に使用
void SkinningModel::StaticImGuiUpdate() {

#ifdef _DEBUG

	ImGui::Begin("Static model Settings");
	ImGui::Combo("Blend", (int*)&currentBlendMode_, BlendTexts, IM_ARRAYSIZE(BlendTexts));
	ImGui::End();

#endif // _DEBUG

}

void SkinningModel::SetMesh(const std::string& objFileName) {

	if (MeshManager::GetInstance()->IsExistMesh(objFileName)) {

		mesh_ = MeshManager::GetInstance()->GetMesh(objFileName);

	}
	else {

		//メッシュを登録
		MeshManager::GetInstance()->CreateMesh(objFileName);
		mesh_ = MeshManager::GetInstance()->GetMesh(objFileName);;

	}

	meshFilePath_ = objFileName;

	texture_ = TextureManager::GetInstance()->Load(mesh_->textureFilePath_);

}

void SkinningModel::ImGuiUpdate(const std::string& name) {

#ifdef _DEBUG

	ImGui::Begin(name.c_str());

	if (ImGui::BeginTabBar("Model", ImGuiTabBarFlags_None)) {

		if (ImGui::BeginTabItem("material")) {
			material_->ImGuiUpdate();
			ImGui::EndTabItem();
		}

		ImGui::EndTabBar();
	}

	ImGui::End();

#endif // _DEBUG

}
