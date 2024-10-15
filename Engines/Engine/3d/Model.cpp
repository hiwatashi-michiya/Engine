#include "Model.h"
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
#include "Core/RootSignatureDesc.h"
#include "Core/RootParameter.h"
#include "Core/StaticSampler.h"
#include "Core/InputElement.h"
#include "Core/InputLayout.h"
#include "Core/DescriptorRange.h"

#pragma comment(lib, "dxcompiler.lib")

ID3D12Device* Model::device_ = nullptr;
ID3D12GraphicsCommandList* Model::commandList_ = nullptr;
ID3D12RootSignature* Model::rootSignature_ = nullptr;
ID3D12PipelineState* Model::pipelineStates_[Model::BlendMode::kCountBlend] = { nullptr };
//ID3D12PipelineState* Model::pipelineState_ = nullptr;
IDxcBlob* Model::vs3dBlob_ = nullptr;
IDxcBlob* Model::ps3dBlob_ = nullptr;
int Model::modelNumber_ = 0;
Model::BlendMode Model::currentBlendMode_ = Model::BlendMode::kNormal;
const char* Model::BlendTexts[Model::BlendMode::kCountBlend] = { "Normal", "Add", "Subtract", "Multiply", "Screen" };

void Model::StaticInitialize(ID3D12Device* device) {

	assert(device);

	HRESULT hr;

	device_ = device;

	//Shaderをコンパイルする
	vs3dBlob_ = ShaderManager::GetInstance()->CompileShader(L"./Resources/shaders/Object3d.VS.hlsl", ShaderManager::kVS, "VS3D");

	ps3dBlob_ = ShaderManager::GetInstance()->CompileShader(L"./Resources/shaders/Object3d.PS.hlsl", ShaderManager::kPS, "PS3D");

	//頂点レイアウト
	ML_InputElement inputElement{};
	inputElement.SetSize(3);
	inputElement.SetElement("POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, D3D12_APPEND_ALIGNED_ELEMENT, 0);
	inputElement.SetElement("TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, D3D12_APPEND_ALIGNED_ELEMENT, 1);
	inputElement.SetElement("NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, D3D12_APPEND_ALIGNED_ELEMENT, 2);

	ML_InputLayout inputLayout{};
	inputLayout.SetElements(inputElement.Get());

	//RootSignature作成
	ML_RootSignatureDesc rootSignatureDesc{};
	rootSignatureDesc.SetFlags(D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

	ML_DescriptorRange descriptorRange{};
	descriptorRange.SetSize(1);
	descriptorRange.SetDescriptorRange(0, 1, D3D12_DESCRIPTOR_RANGE_TYPE_SRV, D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND, 0);

	ML_DescriptorRange descriptorRangeMask{};
	descriptorRangeMask.SetSize(1);
	descriptorRangeMask.SetDescriptorRange(1, 1, D3D12_DESCRIPTOR_RANGE_TYPE_SRV, D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND, 0);

	ML_RootParameter rootParameter{};
	rootParameter.SetSize(7);
	rootParameter.SetRootParameter(D3D12_ROOT_PARAMETER_TYPE_CBV, D3D12_SHADER_VISIBILITY_PIXEL, 0, 0);
	rootParameter.SetRootParameter(D3D12_ROOT_PARAMETER_TYPE_CBV, D3D12_SHADER_VISIBILITY_VERTEX, 0, 1);
	rootParameter.SetRootParameter(D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE, D3D12_SHADER_VISIBILITY_PIXEL, descriptorRange.Get(), 2);
	rootParameter.SetRootParameter(D3D12_ROOT_PARAMETER_TYPE_CBV, D3D12_SHADER_VISIBILITY_PIXEL, 1, 3);
	rootParameter.SetRootParameter(D3D12_ROOT_PARAMETER_TYPE_CBV, D3D12_SHADER_VISIBILITY_PIXEL, 2, 4);
	rootParameter.SetRootParameter(D3D12_ROOT_PARAMETER_TYPE_CBV, D3D12_SHADER_VISIBILITY_PIXEL, 3, 5);
	rootParameter.SetRootParameter(D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE, D3D12_SHADER_VISIBILITY_PIXEL, descriptorRangeMask.Get(), 6);

	rootSignatureDesc.SetRootParameter(rootParameter.Get());

	//Samplerの設定
	ML_StaticSampler staticSampler{};
	staticSampler.SetSize(1);
	staticSampler.SetSampler(D3D12_FILTER_MIN_MAG_MIP_LINEAR, D3D12_TEXTURE_ADDRESS_MODE_WRAP,
		D3D12_COMPARISON_FUNC_NEVER, D3D12_FLOAT32_MAX, 0, D3D12_SHADER_VISIBILITY_PIXEL, 0);

	rootSignatureDesc.SetSamplers(staticSampler.Get());

	//シリアライズしてバイナリにする
	Microsoft::WRL::ComPtr<ID3DBlob> signatureBlob = nullptr;
	Microsoft::WRL::ComPtr<ID3DBlob> errorBlob = nullptr;
	hr = D3D12SerializeRootSignature(&rootSignatureDesc.Get(),
		D3D_ROOT_SIGNATURE_VERSION_1, &signatureBlob, &errorBlob);
	if (FAILED(hr)) {
		Log(reinterpret_cast<char*>(errorBlob->GetBufferPointer()));
		assert(false);
	}

	RootSignatureManager::GetInstance()->CreateRootSignature(signatureBlob, "RootSignature3D");

	rootSignature_ = RootSignatureManager::GetInstance()->GetRootSignature("RootSignature3D");

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
	graphicsPipelineStateDesc.InputLayout = inputLayout.Get(); //InputLayout
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

	PipelineManager::GetInstance()->CreatePipeLine(graphicsPipelineStateDesc, "PipelineNormal3D");

	pipelineStates_[kNormal] = PipelineManager::GetInstance()->GetPipeline("PipelineNormal3D");

	//加算
	blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_ONE;
	graphicsPipelineStateDesc.BlendState = blendDesc; //BlendState

	PipelineManager::GetInstance()->CreatePipeLine(graphicsPipelineStateDesc, "PipelineAdd3D");

	pipelineStates_[kAdd] = PipelineManager::GetInstance()->GetPipeline("PipelineAdd3D");

	//減算
	blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_SUBTRACT;
	blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_ONE;
	graphicsPipelineStateDesc.BlendState = blendDesc; //BlendState

	PipelineManager::GetInstance()->CreatePipeLine(graphicsPipelineStateDesc, "PipelineSubtract3D");

	pipelineStates_[kSubtract] = PipelineManager::GetInstance()->GetPipeline("PipelineSubtract3D");

	//乗算
	blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_ZERO;
	blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_SRC_COLOR;
	graphicsPipelineStateDesc.BlendState = blendDesc; //BlendState

	PipelineManager::GetInstance()->CreatePipeLine(graphicsPipelineStateDesc, "PipelineMultiply3D");

	pipelineStates_[kMultiply] = PipelineManager::GetInstance()->GetPipeline("PipelineMultiply3D");

	//スクリーン
	blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_INV_DEST_COLOR;
	blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_ONE;
	graphicsPipelineStateDesc.BlendState = blendDesc; //BlendState

	PipelineManager::GetInstance()->CreatePipeLine(graphicsPipelineStateDesc, "PipelineScreen3D");

	pipelineStates_[kScreen] = PipelineManager::GetInstance()->GetPipeline("PipelineScreen3D");

	currentBlendMode_ = BlendMode::kNormal;

	//メッシュの静的初期化
	Mesh::StaticInitialize(device);
	//マテリアルの静的初期化
	Material::StaticInitialize(device);

}

Model* Model::Create(const std::string& filename) {

	Model* model = new Model();

	model->Initialize(filename);

	return model;

}

void Model::Initialize(const std::string& filename) {

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

void Model::LoadAnimation(const std::string& filename) {

	//インスタンス生成
	if (!animation_) {

		animation_ = std::make_unique<Animation>();

	}

	*animation_ = LoadAnimationFile(filename);

	if (!skeleton_) {

		skeleton_ = std::make_unique<Skeleton>();

	}

	*skeleton_ = CreateSkeleton(mesh_->modelData_.rootNode);

}

void Model::ResetAnimation() {

	//アニメーションが存在している時のみリセットを行う
	if (animation_ and animation_->nodeAnimations.size() != 0) {

		animationTime_ = 0.0f;
		NodeAnimation& rootNodeAnimation = animation_->nodeAnimations[mesh_->modelData_.rootNode.name]; //rootNodeのanimationを取得
		Vector3 translate = CalculateValue(rootNodeAnimation.translate.keyFrames, animationTime_);
		Quaternion rotate = CalculateValue(rootNodeAnimation.rotate.keyFrames, animationTime_);
		Vector3 scale = CalculateValue(rootNodeAnimation.scale.keyFrames, animationTime_);
		localMatrix_ = MakeAffineMatrix(scale, rotate, translate);

	}

}

void Model::UpdateAnimation() {

	isEndAnimation_ = false;

	//アニメーションが存在していて、再生フラグが立っている時
	if (animation_ and isStartAnimation_ and animation_->nodeAnimations.size() != 0) {

		//現在のアニメーションタイムをアニメーション速度分加算
		animationTime_ += animationSpeed_ / 60.0f;

		//アニメーションタイムが全体の尺を超えていたら終点とみなす
		if (animationTime_ >= animation_->duration) {
			
			animationTime_ = animation_->duration;

			//ループしなければフラグを降ろす
			if (!isLoop_) {
				isStartAnimation_ = false;
			}

			isEndAnimation_ = true;
		}

		//アニメーションの時間調整
		animationTime_ = std::fmod(animationTime_, animation_->duration);
		
		if (skeleton_) {

			ApplyAnimation(*skeleton_, *animation_, animationTime_);
			UpdateSkeleton(*skeleton_);

		}

		NodeAnimation& rootNodeAnimation = animation_->nodeAnimations[mesh_->modelData_.rootNode.name]; //rootNodeのanimationを取得
		Vector3 translate = CalculateValue(rootNodeAnimation.translate.keyFrames, animationTime_);
		Quaternion rotate = CalculateValue(rootNodeAnimation.rotate.keyFrames, animationTime_);
		Vector3 scale = CalculateValue(rootNodeAnimation.scale.keyFrames, animationTime_);
		localMatrix_ = MakeAffineMatrix(scale, rotate, translate);

	}

}

void Model::PreDraw(ID3D12GraphicsCommandList* commandList) {

	assert(commandList_ == nullptr);

	commandList_ = commandList;

	//PSO設定
	commandList_->SetPipelineState(pipelineStates_[currentBlendMode_]);
	//ルートシグネチャを設定
	commandList_->SetGraphicsRootSignature(rootSignature_);
	//プリミティブ形状を設定
	commandList_->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

}

void Model::PostDraw() {

	commandList_ = nullptr;

	modelNumber_ = 0;

}

void Model::Draw(Camera* camera) {

	Matrix4x4 worldViewProjectionMatrix;

	worldViewProjectionMatrix = localMatrix_ * worldMatrix_ * camera->matViewProjection_;
	matTransformMap_->WVP = worldViewProjectionMatrix;
	matTransformMap_->World = localMatrix_ * worldMatrix_;
	matTransformMap_->WorldInverseTranspose = Transpose(Inverse(localMatrix_ * worldMatrix_));

	cameraMap_->worldPosition = camera->GetWorldPosition();

	//PSO設定
	/*commandList_->SetPipelineState(pipelineStates_[currentBlendMode_]);*/

	//カメラ設定
	commandList_->SetGraphicsRootConstantBufferView(4, cameraBuff_->GetGPUVirtualAddress());
	commandList_->SetGraphicsRootConstantBufferView(1, matBuff_->GetGPUVirtualAddress());

	commandList_->SetGraphicsRootDescriptorTable(2, texture_->srvHandleGPU);
	commandList_->SetGraphicsRootDescriptorTable(6, maskTexture_->srvHandleGPU);

	//描画
	material_->SetCommandMaterial(commandList_);

	mesh_->SetCommandMesh(commandList_);

}

void Model::Draw(const Matrix4x4& localMatrix, Camera* camera) {

	Matrix4x4 worldViewProjectionMatrix = localMatrix * worldMatrix_* camera->matViewProjection_;
	matTransformMap_->WVP = worldViewProjectionMatrix;
	matTransformMap_->World = localMatrix * worldMatrix_;
	matTransformMap_->WorldInverseTranspose = Transpose(Inverse(localMatrix * worldMatrix_));

	cameraMap_->worldPosition = camera->GetWorldPosition();

	//ルートシグネチャを設定
	commandList_->SetGraphicsRootSignature(rootSignature_);
	//プリミティブ形状を設定
	commandList_->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//カメラ設定
	commandList_->SetGraphicsRootConstantBufferView(4, cameraBuff_->GetGPUVirtualAddress());
	commandList_->SetGraphicsRootConstantBufferView(1, matBuff_->GetGPUVirtualAddress());

	commandList_->SetGraphicsRootDescriptorTable(2, texture_->srvHandleGPU);

	//描画
	material_->SetCommandMaterial(commandList_);

	mesh_->SetCommandMesh(commandList_);

}

void Model::Finalize() {

}

//デバッグ時に使用
void Model::StaticImGuiUpdate() {

#ifdef _DEBUG

	ImGui::Begin("Static model Settings");
	ImGui::Combo("Blend", (int*)&currentBlendMode_, BlendTexts, IM_ARRAYSIZE(BlendTexts));
	ImGui::End();

#endif // _DEBUG

}

void Model::SetMesh(const std::string& objFileName) {

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

void Model::SetTexture(const std::string& name) {

	texture_ = TextureManager::GetInstance()->Load(name);

}

void Model::ImGuiUpdate(const std::string& name) {

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
