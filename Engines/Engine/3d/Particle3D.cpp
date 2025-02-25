#include "Particle3D.h"
#include <cassert>
#include "Engine/Convert.h"
#include "Engine/Drawing/ShaderManager.h"
#include "Buffer/BufferResource.h"
#include "Drawing/RootSignatureManager.h"
#include "Drawing/PipelineManager.h"
#include <sstream>
#include "Drawing/ImGuiManager.h"
#include "Drawing/MeshManager.h"
#include "Drawing/RenderManager.h"
#include "Core/RootSignatureDesc.h"
#include "Core/RootParameter.h"
#include "Core/StaticSampler.h"
#include "Core/InputElement.h"
#include "Core/InputLayout.h"
#include "Core/DescriptorRange.h"

#pragma comment(lib, "dxcompiler.lib")

ID3D12Device* Particle3D::device_ = nullptr;
ID3D12GraphicsCommandList* Particle3D::commandList_ = nullptr;
ID3D12RootSignature* Particle3D::rootSignature_ = nullptr;
ID3D12PipelineState* Particle3D::particlePipelineStates_[Particle3D::BlendMode::kCountBlend] = { nullptr };
//ID3D12PipelineState* Particle3D::pipelineState_ = nullptr;
IDxcBlob* Particle3D::vs3dParticleBlob_ = nullptr;
IDxcBlob* Particle3D::ps3dParticleBlob_ = nullptr;
Particle3D::BlendMode Particle3D::currentBlendMode_ = Particle3D::BlendMode::kNormal;
std::unordered_map<std::string, std::shared_ptr<Mesh>> Particle3D::meshes_;
const char* Particle3D::BlendTexts[Particle3D::BlendMode::kCountBlend] = { "Normal", "Add", "Subtract", "Multiply", "Screen" };

void Particle3D::StaticInitialize(ID3D12Device* device) {

	assert(device);

	HRESULT hr;

	device_ = device;

	//Shaderをコンパイルする
	vs3dParticleBlob_ = ShaderManager::GetInstance()->CompileShader(L"./Resources/shaders/Particle3d.VS.hlsl", ShaderManager::kVS, "VSParticle3D");

	ps3dParticleBlob_ = ShaderManager::GetInstance()->CompileShader(L"./Resources/shaders/Particle3d.PS.hlsl", ShaderManager::kPS, "PSParticle3D");
	
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

	ML_DescriptorRange descriptorRangeForInstancing{};
	descriptorRangeForInstancing.SetSize(1);
	descriptorRangeForInstancing.SetDescriptorRange(0, 1, D3D12_DESCRIPTOR_RANGE_TYPE_SRV, D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND, 0);

	ML_RootParameter rootParameters{};
	rootParameters.SetSize(4);
	rootParameters.SetRootParameter(D3D12_ROOT_PARAMETER_TYPE_CBV, D3D12_SHADER_VISIBILITY_PIXEL, 0, 0);
	rootParameters.SetRootParameter(D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE, D3D12_SHADER_VISIBILITY_VERTEX, descriptorRangeForInstancing.Get(), 1);
	rootParameters.SetRootParameter(D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE, D3D12_SHADER_VISIBILITY_PIXEL, descriptorRange.Get(), 2);
	rootParameters.SetRootParameter(D3D12_ROOT_PARAMETER_TYPE_CBV, D3D12_SHADER_VISIBILITY_PIXEL, 1, 3);

	rootSignatureDesc.SetRootParameter(rootParameters.Get());

	ML_StaticSampler staticSamplers{};
	staticSamplers.SetSize(1);
	staticSamplers.SetSampler(D3D12_FILTER_MIN_MAG_MIP_LINEAR, D3D12_TEXTURE_ADDRESS_MODE_WRAP,
		D3D12_COMPARISON_FUNC_NEVER, D3D12_FLOAT32_MAX, 0, D3D12_SHADER_VISIBILITY_PIXEL, 0);

	rootSignatureDesc.SetSamplers(staticSamplers.Get());

	//シリアライズしてバイナリにする
	Microsoft::WRL::ComPtr<ID3DBlob> signatureBlob = nullptr;
	Microsoft::WRL::ComPtr<ID3DBlob> errorBlob = nullptr;
	hr = D3D12SerializeRootSignature(&rootSignatureDesc.Get(),
		D3D_ROOT_SIGNATURE_VERSION_1, &signatureBlob, &errorBlob);
	if (FAILED(hr)) {
		Log(reinterpret_cast<char*>(errorBlob->GetBufferPointer()));
		assert(false);
	}

	RootSignatureManager::GetInstance()->CreateRootSignature(signatureBlob, "RootSignatureParticle3D");

	rootSignature_ = RootSignatureManager::GetInstance()->GetRootSignature("RootSignatureParticle3D");

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
	graphicsPipelineStateDesc.InputLayout = inputLayout.Get(); //InputLayout
	graphicsPipelineStateDesc.VS = { vs3dParticleBlob_->GetBufferPointer(),
	vs3dParticleBlob_->GetBufferSize() }; //VertexShader
	graphicsPipelineStateDesc.PS = { ps3dParticleBlob_->GetBufferPointer(),
	ps3dParticleBlob_->GetBufferSize() }; //PixelShader
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
	
	PipelineManager::GetInstance()->CreatePipeLine(graphicsPipelineStateDesc, "PipelineNormalParticle3D");

	particlePipelineStates_[kNormal] = PipelineManager::GetInstance()->GetPipeline("PipelineNormalParticle3D");

	//加算
	blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_ONE;
	graphicsPipelineStateDesc.BlendState = blendDesc; //BlendState
	
	PipelineManager::GetInstance()->CreatePipeLine(graphicsPipelineStateDesc, "PipelineAddParticle3D");

	particlePipelineStates_[kAdd] = PipelineManager::GetInstance()->GetPipeline("PipelineAddParticle3D");

	//減算
	blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_SUBTRACT;
	blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_ONE;
	graphicsPipelineStateDesc.BlendState = blendDesc; //BlendState
	
	PipelineManager::GetInstance()->CreatePipeLine(graphicsPipelineStateDesc, "PipelineSubtractParticle3D");

	particlePipelineStates_[kSubtract] = PipelineManager::GetInstance()->GetPipeline("PipelineSubtractParticle3D");

	//乗算
	blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_ZERO;
	blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_SRC_COLOR;
	graphicsPipelineStateDesc.BlendState = blendDesc; //BlendState
	
	PipelineManager::GetInstance()->CreatePipeLine(graphicsPipelineStateDesc, "PipelineMultiplyParticle3D");

	particlePipelineStates_[kMultiply] = PipelineManager::GetInstance()->GetPipeline("PipelineMultiplyParticle3D");

	//スクリーン
	blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_INV_DEST_COLOR;
	blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_ONE;
	graphicsPipelineStateDesc.BlendState = blendDesc; //BlendState
	
	PipelineManager::GetInstance()->CreatePipeLine(graphicsPipelineStateDesc, "PipelineScreenParticle3D");

	particlePipelineStates_[kScreen] = PipelineManager::GetInstance()->GetPipeline("PipelineScreenParticle3D");

	currentBlendMode_ = BlendMode::kNormal;

}

Particle3D* Particle3D::Create(const std::string& filename, uint32_t instanceCount) {

	Particle3D* particle3d = new Particle3D();

	particle3d->Initialize(filename, instanceCount);

	return particle3d;

}

void Particle3D::Initialize(const std::string& filename, uint32_t instanceCount) {

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
	worldMatrices.resize(maxInstanceCount_);
	velocities_.resize(maxInstanceCount_);
	isActive_.resize(maxInstanceCount_);
	lifeTimes_.resize(maxInstanceCount_);

	for (uint32_t i = 0; i < maxInstanceCount_; i++) {
		transforms_[i] = std::make_shared<Transform>();
		colors_[i] = { 1.0f,1.0f,1.0f,1.0f };
		worldMatrices[i] = MakeIdentity4x4();
	}

	instanceCount_ = maxInstanceCount_;

	//transformMatrix
	{

		matBuff_ = CreateBufferResource(device_, sizeof(ParticleForGPU) * maxInstanceCount_);

		matBuff_->Map(0, nullptr, reinterpret_cast<void**>(&matTransformMap_));

		//インスタンシングの数だけループ
		for (uint32_t i = 0; i < maxInstanceCount_; i++) {
			matTransformMap_[i].WVP = MakeIdentity4x4();
			matTransformMap_[i].World = MakeIdentity4x4();
			matTransformMap_[i].color = { 1.0f,1.0f,1.0f,1.0f };
		}

		matBuff_->Unmap(0, nullptr);

	}

	//インスタンシングリソース設定
	{

		instancingResource_ = TextureManager::GetInstance()->SetInstancingResource(maxInstanceCount_, matBuff_);

	}

}

void Particle3D::PreDraw(ID3D12GraphicsCommandList* commandList) {

	assert(commandList_ == nullptr);

	commandList_ = commandList;

	//PSO設定
	commandList_->SetPipelineState(particlePipelineStates_[currentBlendMode_]);
	//ルートシグネチャを設定
	commandList_->SetGraphicsRootSignature(rootSignature_);
	//プリミティブ形状を設定
	commandList_->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

}

void Particle3D::PostDraw() {

	commandList_ = nullptr;

}

void Particle3D::Draw(Camera* camera) {

	matBillboard_ = camera->matWorld_;

	matBillboard_.m[3][0] = 0.0f;
	matBillboard_.m[3][1] = 0.0f;
	matBillboard_.m[3][2] = 0.0f;

	instanceCount_ = std::clamp(instanceCount_, uint32_t(0), maxInstanceCount_);

	for (uint32_t i = 0; i < instanceCount_; i++) {

		//アクティブ状態でない場合、スケールを0にして表示しない
		if (not isActive_[i]) {
			transforms_[i]->scale_ = Vector3::Zero();
		}

		if (isBillboard_) {
			worldMatrices[i] = MakeScaleMatrix(transforms_[i]->scale_) * matBillboard_ * MakeTranslateMatrix(transforms_[i]->translate_);
		}
		else {
			worldMatrices[i] = MakeAffineMatrix(transforms_[i]->scale_, transforms_[i]->rotateQuaternion_, transforms_[i]->translate_);
		}

		/*Matrix4x4 worldMatrix = worldTransform[i].matWorld_;*/
		Matrix4x4 worldViewProjectionMatrix = worldMatrices[i] * camera->matViewProjection_;
		matTransformMap_[i].WVP = worldViewProjectionMatrix;
		matTransformMap_[i].World = worldMatrices[i];
		matTransformMap_[i].WorldInverseTranspose = Transpose(Inverse(worldMatrices[i]));
		matTransformMap_[i].color = colors_[i];

	}

	RenderManager::GetInstance()->AddParticle3D(this);

}

void Particle3D::Render()
{

	commandList_->SetGraphicsRootDescriptorTable(1, instancingResource_.srvHandleGPU);

	commandList_->SetGraphicsRootDescriptorTable(2, texture_->srvHandleGPU);

	//描画
	material_->SetCommandMaterialForParticle(commandList_);

	mesh_->SetCommandMesh(commandList_, instanceCount_);

}

void Particle3D::Finalize() {

	

}

void Particle3D::ImGuiUpdate() {


}

void Particle3D::SetTexture(const std::string& name)
{
	texture_ = TextureManager::GetInstance()->Load(name);
	texturePath_ = name;
}

//デバッグ時に使用
void Particle3D::StaticImGuiUpdate() {

#ifdef _DEBUG

	ImGui::Begin("Static particle Settings");
	ImGui::Combo("Blend", (int*)&currentBlendMode_, BlendTexts, IM_ARRAYSIZE(BlendTexts));
	ImGui::End();

#endif // _DEBUG

}

bool Particle3D::IsAnyActive() {

	for (uint32_t i = 0; i < instanceCount_; i++) {

		if (isActive_[i]) {
			return true;
		}

	}

	return false;

}
