#include <filesystem>
#include <cassert>
#include <fstream>
#include "Engine/Convert.h"
#include "ModelManager.h"
#include "ShaderManager.h"
#include "RootSignatureManager.h"
#include "PipelineManager.h"
#include "Core/RootSignatureDesc.h"
#include "Core/RootParameter.h"
#include "Core/StaticSampler.h"
#include "Core/InputElement.h"
#include "Core/InputLayout.h"
#include "Core/DescriptorRange.h"

ModelManager* ModelManager::GetInstance()
{
	static ModelManager instance;
	return &instance;
}

void ModelManager::Initialize()
{

	HRESULT hr;

	//Shaderをコンパイルする
	IDxcBlob* vs3dBlob_ = ShaderManager::GetInstance()->CompileShader(L"./Resources/shaders/InstancingObject3d.VS.hlsl", ShaderManager::kVS, "VSInstancing3D");

	IDxcBlob* ps3dBlob_ = ShaderManager::GetInstance()->CompileShader(L"./Resources/shaders/InstancingObject3d.PS.hlsl", ShaderManager::kPS, "PSInstancing3D");

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

	ML_DescriptorRange descriptorRangeForInstancing{};
	descriptorRangeForInstancing.SetSize(1);
	descriptorRangeForInstancing.SetDescriptorRange(0, 1, D3D12_DESCRIPTOR_RANGE_TYPE_SRV, D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND, 0);

	ML_RootParameter rootParameter{};
	rootParameter.SetSize(8);
	rootParameter.SetRootParameter(D3D12_ROOT_PARAMETER_TYPE_CBV, D3D12_SHADER_VISIBILITY_PIXEL, 0, 0);
	rootParameter.SetRootParameter(D3D12_ROOT_PARAMETER_TYPE_CBV, D3D12_SHADER_VISIBILITY_VERTEX, 0, 1);
	rootParameter.SetRootParameter(D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE, D3D12_SHADER_VISIBILITY_PIXEL, descriptorRange.Get(), 2);
	rootParameter.SetRootParameter(D3D12_ROOT_PARAMETER_TYPE_CBV, D3D12_SHADER_VISIBILITY_PIXEL, 1, 3);
	rootParameter.SetRootParameter(D3D12_ROOT_PARAMETER_TYPE_CBV, D3D12_SHADER_VISIBILITY_PIXEL, 2, 4);
	rootParameter.SetRootParameter(D3D12_ROOT_PARAMETER_TYPE_CBV, D3D12_SHADER_VISIBILITY_PIXEL, 3, 5);
	rootParameter.SetRootParameter(D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE, D3D12_SHADER_VISIBILITY_PIXEL, descriptorRangeMask.Get(), 6);
	rootParameter.SetRootParameter(D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE, D3D12_SHADER_VISIBILITY_VERTEX, descriptorRangeForInstancing.Get(), 7);

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

}

void ModelManager::AddModel(const std::string& fileName)
{

	//モデルが生成されていたら早期リターン
	if (models_.find(fileName) != models_.end()) {
		return;
	}

	std::unique_ptr<InstancingModel> newModel;
	newModel.reset(InstancingModel::Create(fileName));

	models_[fileName] = std::move(newModel);

}

void ModelManager::PreDraw(ID3D12GraphicsCommandList* commandList)
{

	assert(commandList_ == nullptr);

	commandList_ = commandList;

	//PSO設定
	commandList_->SetPipelineState(pipelineStates_[currentBlendMode_]);
	//ルートシグネチャを設定
	commandList_->SetGraphicsRootSignature(rootSignature_);
	//プリミティブ形状を設定
	commandList_->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

}

void ModelManager::PostDraw()
{
	commandList_ = nullptr;
}

void ModelManager::Render()
{
	//描画
	for (const auto& pair : models_) {
		pair.second->Render(commandList_);
	}

}

InstancingModel* ModelManager::GetModel(const std::string& fileName)
{

	//モデルが生成されていたら返す
	if (models_.find(fileName) != models_.end()) {
		return models_[fileName].get();
	}

	return nullptr;
}
