#include "Particle2D.h"
#include <cassert>
#include "Engine/Convert.h"
#include "Engine/Drawing/ShaderManager.h"
#include "Buffer/BufferResource.h"
#include "Drawing/RootSignatureManager.h"
#include "Drawing/PipelineManager.h"
#include <sstream>
#include "Drawing/ImGuiManager.h"
#include "Model.h"
#include "Drawing/MeshManager.h"
#include "Drawing/RenderManager.h"

#pragma comment(lib, "dxcompiler.lib")

ID3D12Device* Particle2D::device_ = nullptr;
ID3D12GraphicsCommandList* Particle2D::commandList_ = nullptr;
ID3D12RootSignature* Particle2D::rootSignature_ = nullptr;
ID3D12PipelineState* Particle2D::particlePipelineStates_ = nullptr;

void Particle2D::StaticInitialize(ID3D12Device* device) {

	assert(device);

	HRESULT hr;

	device_ = device;

	//Shaderをコンパイルする
	IDxcBlob* vs2dParticleBlob = ShaderManager::GetInstance()->CompileShader(L"./Resources/shaders/Particle2d.VS.hlsl", ShaderManager::kVS, "VSParticle2D");

	IDxcBlob* ps2dParticleBlob = ShaderManager::GetInstance()->CompileShader(L"./Resources/shaders/Particle2d.PS.hlsl", ShaderManager::kPS, "PSParticle2D");
	
	//頂点レイアウト
	D3D12_INPUT_ELEMENT_DESC inputElementDescs[3] = {};
	inputElementDescs[0].SemanticName = "POSITION";
	inputElementDescs[0].SemanticIndex = 0;
	inputElementDescs[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	inputElementDescs[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	inputElementDescs[1].SemanticName = "TEXCOORD";
	inputElementDescs[1].SemanticIndex = 0;
	inputElementDescs[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	inputElementDescs[1].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	D3D12_INPUT_LAYOUT_DESC inputLayoutDesc{};
	inputLayoutDesc.pInputElementDescs = inputElementDescs;
	inputLayoutDesc.NumElements = _countof(inputElementDescs);

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

	D3D12_DESCRIPTOR_RANGE descriptorRangeForInstancing[1] = {};
	descriptorRangeForInstancing[0].BaseShaderRegister = 0;
	descriptorRangeForInstancing[0].NumDescriptors = 1;
	descriptorRangeForInstancing[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	descriptorRangeForInstancing[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	//ルートパラメータ作成
	D3D12_ROOT_PARAMETER rootParameters[4]{};
	rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	rootParameters[0].Descriptor.ShaderRegister = 0;

	rootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE; //DescriptorTableを使う
	rootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX; //VertexShaderで使う
	rootParameters[1].DescriptorTable.pDescriptorRanges = descriptorRangeForInstancing; //Tableの中身の配列を指定
	rootParameters[1].DescriptorTable.NumDescriptorRanges = _countof(descriptorRangeForInstancing); //Tableで利用する数

	rootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE; //DescriptorTableを使う
	rootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL; //PixelShaderで使う
	rootParameters[2].DescriptorTable.pDescriptorRanges = descriptorRange; //Tableの中身の配列を指定
	rootParameters[2].DescriptorTable.NumDescriptorRanges = _countof(descriptorRange); //Tableで利用する数

	//平行光源
	rootParameters[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV; //CBVを使う
	rootParameters[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL; //PixelShaderで使う
	rootParameters[3].Descriptor.ShaderRegister = 1; //レジスタ番号1を使う

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

	RootSignatureManager::GetInstance()->CreateRootSignature(signatureBlob, "RootSignatureParticle2D");

	rootSignature_ = RootSignatureManager::GetInstance()->GetRootSignature("RootSignatureParticle2D");

	//Blendstateの設定
	D3D12_BLEND_DESC blendDesc{};
	//全ての色要素を書き込む
	blendDesc.RenderTarget[0].RenderTargetWriteMask =
		D3D12_COLOR_WRITE_ENABLE_ALL;
	blendDesc.RenderTarget[0].BlendEnable = TRUE;
	blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
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
	graphicsPipelineStateDesc.VS = { vs2dParticleBlob->GetBufferPointer(),
	vs2dParticleBlob->GetBufferSize() }; //VertexShader
	graphicsPipelineStateDesc.PS = { ps2dParticleBlob->GetBufferPointer(),
	ps2dParticleBlob->GetBufferSize() }; //PixelShader
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
	depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
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
	
	PipelineManager::GetInstance()->CreatePipeLine(graphicsPipelineStateDesc, "PipelineNormalParticle2D");

	particlePipelineStates_ = PipelineManager::GetInstance()->GetPipeline("PipelineNormalParticle2D");

}

Particle2D* Particle2D::Create(const std::string& filename, uint32_t instanceCount) {

	Particle2D* particle2d = new Particle2D();

	particle2d->Initialize(filename, instanceCount);

	return particle2d;

}

void Particle2D::Initialize(const std::string& filename, uint32_t instanceCount) {

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

	texture_ = TextureManager::GetInstance()->Load(mesh_->textureFilePath_);
	texturePath_ = mesh_->textureFilePath_;

	maxInstanceCount_ = instanceCount;

	//トランスフォーム情報をインスタンス数に合わせてリサイズする
	transforms_.resize(maxInstanceCount_);
	colors_.resize(maxInstanceCount_);
	worldMatrices_.resize(maxInstanceCount_);
	velocities_.resize(maxInstanceCount_);
	isActive_.resize(maxInstanceCount_);
	lifeTimes_.resize(maxInstanceCount_);

	for (uint32_t i = 0; i < maxInstanceCount_; i++) {
		transforms_[i] = std::make_shared<Transform>();
		colors_[i] = { 1.0f,1.0f,1.0f,1.0f };
		worldMatrices_[i] = MakeIdentity4x4();
	}

	instanceCount_ = maxInstanceCount_;

	//transformMatrix
	{

		matBuff_ = CreateBufferResource(device_, sizeof(ParticleForGPU) * maxInstanceCount_);

		matBuff_->Map(0, nullptr, reinterpret_cast<void**>(&matTransformMap_));

		//インスタンシングの数だけループ
		for (uint32_t i = 0; i < maxInstanceCount_; i++) {
			matTransformMap_[i].WVP = MakeIdentity4x4();
			matTransformMap_[i].color = { 1.0f,1.0f,1.0f,1.0f };
		}

		matBuff_->Unmap(0, nullptr);

	}

	//インスタンシングリソース設定
	{

		instancingResource_ = TextureManager::GetInstance()->SetInstancingResource(maxInstanceCount_, matBuff_);

	}

	//頂点バッファ
	{

		vertBuff_ = CreateBufferResource(device_, sizeof(VertexData) * 4);

		//頂点バッファビュー設定
		vbView_.BufferLocation = vertBuff_->GetGPUVirtualAddress();
		vbView_.SizeInBytes = sizeof(VertexData) * 4;
		vbView_.StrideInBytes = sizeof(VertexData);

		//マッピングしてデータ転送
		vertBuff_->Map(0, nullptr, reinterpret_cast<void**>(&vertMap_));

		//左下
		vertMap_[0].position = { 0.0f, 1.0f, 0.0f,1.0f };
		vertMap_[0].texcoord = { 0.0f,1.0f };
		//左上
		vertMap_[1].position = { 0.0f,0.0f, 0.0f,1.0f };
		vertMap_[1].texcoord = { 0.0f,0.0f };
		//右下
		vertMap_[2].position = { 1.0f, 1.0f, 0.0f,1.0f };
		vertMap_[2].texcoord = { 1.0f,1.0f };
		//右上
		vertMap_[3].position = { 1.0f, 0.0f, 0.0f,1.0f };
		vertMap_[3].texcoord = { 1.0f,0.0f };

		//アンマップ
		vertBuff_->Unmap(0, nullptr);

	}
	//インデックスバッファ
	{

		indexBuff_ = CreateBufferResource(device_, sizeof(uint32_t) * 6);

		//インデックスバッファビュー設定
		ibView_.BufferLocation = indexBuff_->GetGPUVirtualAddress();
		ibView_.SizeInBytes = sizeof(uint32_t) * 6;
		ibView_.Format = DXGI_FORMAT_R32_UINT;

		//マッピングしてデータ転送
		indexBuff_->Map(0, nullptr, reinterpret_cast<void**>(&indexMap_));

		indexMap_[0] = 0;
		indexMap_[1] = 1;
		indexMap_[2] = 2;
		indexMap_[3] = 1;
		indexMap_[4] = 3;
		indexMap_[5] = 2;

		//アンマップ
		indexBuff_->Unmap(0, nullptr);

	}

	//定数バッファ
	{

		constBuff_ = CreateBufferResource(device_, sizeof(Material2D));

		//マッピングしてデータ転送
		constBuff_->Map(0, nullptr, reinterpret_cast<void**>(&constMap_));

		constMap_->color = { 1.0f,1.0f,1.0f,1.0f };
		constMap_->uvTransform = MakeIdentity4x4();

		//アンマップ
		constBuff_->Unmap(0, nullptr);

	}

}

void Particle2D::PreDraw(ID3D12GraphicsCommandList* commandList) {

	assert(commandList_ == nullptr);

	commandList_ = commandList;

	//PSO設定
	commandList_->SetPipelineState(particlePipelineStates_);
	//ルートシグネチャを設定
	commandList_->SetGraphicsRootSignature(rootSignature_);
	//プリミティブ形状を設定
	commandList_->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

}

void Particle2D::PostDraw() {

	commandList_ = nullptr;

}

void Particle2D::Draw(Camera* camera) {


	instanceCount_ = std::clamp(instanceCount_, uint32_t(0), maxInstanceCount_);

	for (uint32_t i = 0; i < instanceCount_; i++) {

		//アクティブ状態でない場合、スケールを0にして表示しない
		if (not isActive_[i]) {
			transforms_[i]->scale_ = Vector3::Zero();
		}

		worldMatrices_[i] = MakeAffineMatrix(transforms_[i]->scale_, transforms_[i]->rotateQuaternion_, transforms_[i]->translate_);

		/*Matrix4x4 worldMatrix = worldTransform[i].matWorld_;*/
		Matrix4x4 worldViewProjectionMatrix = worldMatrices_[i] * camera->matViewProjection_;
		matTransformMap_[i].WVP = worldViewProjectionMatrix;
		matTransformMap_[i].color = colors_[i];

	}

}

void Particle2D::Render()
{

	commandList_->SetGraphicsRootDescriptorTable(1, instancingResource_.srvHandleGPU);

	commandList_->SetGraphicsRootDescriptorTable(2, texture_->srvHandleGPU);

	//描画
	material_->SetCommandMaterialForParticle(commandList_);

	mesh_->SetCommandMesh(commandList_, instanceCount_);

}

void Particle2D::Finalize() {

	

}

void Particle2D::ImGuiUpdate() {


}

void Particle2D::SetTexture(const std::string& name)
{
	texture_ = TextureManager::GetInstance()->Load(name);
	texturePath_ = name;
}


bool Particle2D::IsAnyActive() {

	for (uint32_t i = 0; i < instanceCount_; i++) {

		if (isActive_[i]) {
			return true;
		}

	}

	return false;

}
