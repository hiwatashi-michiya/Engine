#include "LineDrawer.h"
#include <cassert>
#include "Drawing/ShaderManager.h"
#include "Drawing/RootSignatureManager.h"
#include "Drawing/PipelineManager.h"
#include "Buffer/BufferResource.h"
#include "Convert.h"

ID3D12Device* Line::device_ = nullptr;
ID3D12GraphicsCommandList* Line::commandList_ = nullptr;
ID3D12RootSignature* Line::rootSignature_ = nullptr;
ID3D12PipelineState* Line::pipelineState_ = nullptr;

void Line::Initialize(ID3D12Device* device) {

	assert(device);

	HRESULT hr;

	device_ = device;

	IDxcBlob* vsBlob = ShaderManager::GetInstance()->CompileShader(L"./Resources/shaders/Line.VS.hlsl", ShaderManager::kVS, "VSLine");

	IDxcBlob* psBlob = ShaderManager::GetInstance()->CompileShader(L"./Resources/shaders/Line.PS.hlsl", ShaderManager::kPS, "PSLine");

	//頂点レイアウト
	D3D12_INPUT_ELEMENT_DESC inputElementDescs[2] = {};
	inputElementDescs[0].SemanticName = "POSITION";
	inputElementDescs[0].SemanticIndex = 0;
	inputElementDescs[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	inputElementDescs[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	inputElementDescs[1].SemanticName = "COLOR";
	inputElementDescs[1].SemanticIndex = 0;
	inputElementDescs[1].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	inputElementDescs[1].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

	D3D12_INPUT_LAYOUT_DESC inputLayoutDesc{};
	inputLayoutDesc.pInputElementDescs = inputElementDescs;
	inputLayoutDesc.NumElements = _countof(inputElementDescs);

	//RootSignature作成
	D3D12_ROOT_SIGNATURE_DESC descriptionRootSignature{};
	descriptionRootSignature.Flags =
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	//ルートパラメータ作成
	D3D12_ROOT_PARAMETER rootParameters[1]{};
	rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
	rootParameters[0].Descriptor.ShaderRegister = 0;

	descriptionRootSignature.pParameters = rootParameters; //ルートパラメータ配列へのポインタ
	descriptionRootSignature.NumParameters = _countof(rootParameters); //ルートパラメータの長さ

	//シリアライズしてバイナリにする
	Microsoft::WRL::ComPtr<ID3DBlob> signatureBlob = nullptr;
	Microsoft::WRL::ComPtr<ID3DBlob> errorBlob = nullptr;
	hr = D3D12SerializeRootSignature(&descriptionRootSignature,
		D3D_ROOT_SIGNATURE_VERSION_1, &signatureBlob, &errorBlob);
	if (FAILED(hr)) {
		Log(reinterpret_cast<char*>(errorBlob->GetBufferPointer()));
		assert(false);
	}

	RootSignatureManager::GetInstance()->CreateRootSignature(signatureBlob, "RootSignature3DLine");

	rootSignature_ = RootSignatureManager::GetInstance()->GetRootSignature("RootSignature3DLine");

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
	graphicsPipelineStateDesc.VS = { vsBlob->GetBufferPointer(),
	vsBlob->GetBufferSize() }; //VertexShader
	graphicsPipelineStateDesc.PS = { psBlob->GetBufferPointer(),
	psBlob->GetBufferSize() }; //PixelShader
	graphicsPipelineStateDesc.RasterizerState = rasterizerDesc; //RasterizerState
	//書き込むRTVの情報
	graphicsPipelineStateDesc.NumRenderTargets = 1;
	graphicsPipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	//利用するトポロジ(形状)のタイプ、線
	graphicsPipelineStateDesc.PrimitiveTopologyType =
		D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE;
	//どのように画面に色を打ち込むかの設定(気にしなくて良い)
	graphicsPipelineStateDesc.SampleDesc.Count = 1;
	graphicsPipelineStateDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;
	//DepthStencilStateの設定
	D3D12_DEPTH_STENCIL_DESC depthStencilDesc{};
	//Depthの機能を有効化する
	depthStencilDesc.DepthEnable = false;
	//書き込み
	depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	//近ければ描画
	depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;

	//DepthStencilの設定
	graphicsPipelineStateDesc.DepthStencilState = depthStencilDesc;
	graphicsPipelineStateDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;

	//通常
	blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
	graphicsPipelineStateDesc.BlendState = blendDesc; //BlendState

	PipelineManager::GetInstance()->CreatePipeLine(graphicsPipelineStateDesc, "Pipeline3DLine");

	pipelineState_ = PipelineManager::GetInstance()->GetPipeline("Pipeline3DLine");

}

void Line::PreDraw(ID3D12GraphicsCommandList* commandList) {

	assert(commandList_ == nullptr);

	commandList_ = commandList;

	//PSO設定
	commandList_->SetPipelineState(pipelineState_);

}

void Line::PostDraw() {

	commandList_ = nullptr;

}

Line::Line()
{

	start_ = Vector3::Zero();
	end_ = Vector3::Identity();

	color_ = Vector4(1.0f, 1.0f, 1.0f, 1.0f);


	{

		vertexBuff_ = CreateBufferResource(device_, sizeof(LineVertexData) * kVertexNum_);

		//頂点バッファビュー設定
		vbView_.BufferLocation = vertexBuff_->GetGPUVirtualAddress();
		vbView_.SizeInBytes = UINT(sizeof(LineVertexData) * kVertexNum_);
		vbView_.StrideInBytes = sizeof(LineVertexData);

		vertexBuff_->Map(0, nullptr, reinterpret_cast<void**>(&vertexMap_));

		vertexMap_[0].position = Vector4(start_.x, start_.y, start_.z, 1.0f);
		vertexMap_[0].color = color_;

		vertexMap_[1].position = Vector4(end_.x, end_.y, end_.z, 1.0f);
		vertexMap_[1].color = color_;

		vertexBuff_->Unmap(0, nullptr);

	}


	{

		wvpBuff_ = CreateBufferResource(device_, sizeof(Matrix4x4));

		wvpBuff_->Map(0, nullptr, reinterpret_cast<void**>(&wvpMatrix_));

		*wvpMatrix_ = MakeIdentity4x4();

		wvpBuff_->Unmap(0, nullptr);

	}
}

Line::~Line()
{
}

void Line::Draw(Camera* camera) {

	vertexMap_[0].position = Vector4(start_.x, start_.y, start_.z, 1.0f);
	vertexMap_[0].color = color_;

	vertexMap_[1].position = Vector4(end_.x, end_.y, end_.z, 1.0f);
	vertexMap_[1].color = color_;

	*wvpMatrix_ = camera->matViewProjection_;

	commandList_->SetPipelineState(pipelineState_);

	commandList_->SetGraphicsRootSignature(rootSignature_);

	commandList_->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINELIST);

	commandList_->IASetVertexBuffers(0, 1, &vbView_);

	commandList_->SetGraphicsRootConstantBufferView(0, wvpBuff_->GetGPUVirtualAddress());

	commandList_->DrawInstanced(kVertexNum_, 1, 0, 0);

}