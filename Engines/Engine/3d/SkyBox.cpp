#include "Skybox.h"
#include "DirectXSetter.h"
#include "PipelineManager.h"
#include "ShaderManager.h"
#include "RootSignatureManager.h"
#include "Convert.h"
#include <cassert>
#include "BufferResource.h"

ID3D12Device* Skybox::device_ = nullptr;
ID3D12GraphicsCommandList* Skybox::commandList_ = nullptr;
ID3D12RootSignature* Skybox::rootSignature_ = nullptr;
ID3D12PipelineState* Skybox::pipelineState_ = nullptr;

Skybox::Skybox()
{
	Create();
}

Skybox::~Skybox()
{
}

void Skybox::Initialize() {

	HRESULT hr;

	device_ = DirectXSetter::GetInstance()->GetDevice();

	//Shaderをコンパイルする
	IDxcBlob* vs3dBlob = ShaderManager::GetInstance()->CompileShader(L"./Resources/shaders/Skybox.VS.hlsl", ShaderManager::kVS, "VSSkybox");

	IDxcBlob* ps3dBlob = ShaderManager::GetInstance()->CompileShader(L"./Resources/shaders/Skybox.PS.hlsl", ShaderManager::kPS, "PSSkybox");

	//頂点レイアウト
	D3D12_INPUT_ELEMENT_DESC inputElementDescs[1] = {};
	inputElementDescs[0].SemanticName = "POSITION";
	inputElementDescs[0].SemanticIndex = 0;
	inputElementDescs[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	inputElementDescs[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	D3D12_INPUT_LAYOUT_DESC inputLayoutDesc{};
	inputLayoutDesc.pInputElementDescs = inputElementDescs;
	inputLayoutDesc.NumElements = _countof(inputElementDescs);

	//RootSignature作成
	D3D12_ROOT_SIGNATURE_DESC descriptionRootSignature{};
	descriptionRootSignature.Flags =
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	D3D12_DESCRIPTOR_RANGE descriptorRange[1] = {};
	descriptorRange[0].BaseShaderRegister = 0;
	descriptorRange[0].NumDescriptors = 1;
	descriptorRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	descriptorRange[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	//ルートパラメータ作成
	D3D12_ROOT_PARAMETER rootParameters[3]{};

	//マテリアル
	rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	rootParameters[0].Descriptor.ShaderRegister = 0;
	//トランスフォーム
	rootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	rootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
	rootParameters[1].Descriptor.ShaderRegister = 0;
	//テクスチャ
	rootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE; //DescriptorTableを使う
	rootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL; //PixelShaderで使う
	rootParameters[2].DescriptorTable.pDescriptorRanges = descriptorRange; //Tableの中身の配列を指定
	rootParameters[2].DescriptorTable.NumDescriptorRanges = _countof(descriptorRange); //Tableで利用する数

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

	RootSignatureManager::GetInstance()->CreateRootSignature(signatureBlob, "RootSignatureSkybox");

	rootSignature_ = RootSignatureManager::GetInstance()->GetRootSignature("RootSignatureSkybox");

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
	graphicsPipelineStateDesc.VS = { vs3dBlob->GetBufferPointer(),
	vs3dBlob->GetBufferSize() }; //VertexShader
	graphicsPipelineStateDesc.PS = { ps3dBlob->GetBufferPointer(),
	ps3dBlob->GetBufferSize() }; //PixelShader
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
	//z=1なので書き込みを省く
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

	PipelineManager::GetInstance()->CreatePipeLine(graphicsPipelineStateDesc, "PipelineNormalSkybox");

	pipelineState_= PipelineManager::GetInstance()->GetPipeline("PipelineNormalSkybox");

}

void Skybox::PreDraw(ID3D12GraphicsCommandList* commandList) {

	assert(commandList_ == nullptr);

	commandList_ = commandList;

	//PSO設定
	commandList_->SetPipelineState(pipelineState_);

}

void Skybox::PostDraw() {

	commandList_ = nullptr;

}

void Skybox::Create() {

	//頂点バッファ
	{

		vertexBuff_ = CreateBufferResource(device_, sizeof(SkyboxVertexData) * 24);

		vertexBuff_->SetName(L"vertBuff");

		//頂点バッファビュー設定
		vbView_.BufferLocation = vertexBuff_->GetGPUVirtualAddress();
		vbView_.SizeInBytes = UINT(sizeof(SkyboxVertexData) * 24);
		vbView_.StrideInBytes = sizeof(SkyboxVertexData);

		//マッピングしてデータ転送
		vertexBuff_->Map(0, nullptr, reinterpret_cast<void**>(&vertexMap_));

		//右面
		vertexMap_[0].position = { 1.0f,1.0f,1.0f,1.0f };
		vertexMap_[1].position = { 1.0f,1.0f,-1.0f,1.0f };
		vertexMap_[2].position = { 1.0f,-1.0f,1.0f,1.0f };
		vertexMap_[3].position = { 1.0f,-1.0f,-1.0f,1.0f };
		//左面
		vertexMap_[4].position = { -1.0f,1.0f,-1.0f,1.0f };
		vertexMap_[5].position = { -1.0f,1.0f,1.0f,1.0f };
		vertexMap_[6].position = { -1.0f,-1.0f,-1.0f,1.0f };
		vertexMap_[7].position = { -1.0f,-1.0f,1.0f,1.0f };
		//前面
		vertexMap_[8].position = { -1.0f,1.0f,1.0f,1.0f };
		vertexMap_[9].position = { 1.0f,1.0f,1.0f,1.0f };
		vertexMap_[10].position = { -1.0f,-1.0f,1.0f,1.0f };
		vertexMap_[11].position = { 1.0f,-1.0f,1.0f,1.0f };
		//後面
		vertexMap_[12].position = { 1.0f,1.0f,-1.0f,1.0f };
		vertexMap_[13].position = {-1.0f,1.0f,-1.0f,1.0f };
		vertexMap_[14].position = { 1.0f,-1.0f,-1.0f,1.0f };
		vertexMap_[15].position = { -1.0f,-1.0f,-1.0f,1.0f };
		//上面
		vertexMap_[16].position = { -1.0f,1.0f,-1.0f,1.0f };
		vertexMap_[17].position = { 1.0f,1.0f,-1.0f,1.0f };
		vertexMap_[18].position = { -1.0f,1.0f,1.0f,1.0f };
		vertexMap_[19].position = { 1.0f,1.0f,1.0f,1.0f };
		//下面
		vertexMap_[20].position = { -1.0f,-1.0f,1.0f,1.0f };
		vertexMap_[21].position = { 1.0f,-1.0f,1.0f,1.0f };
		vertexMap_[22].position = { -1.0f,-1.0f,-1.0f,1.0f };
		vertexMap_[23].position = { 1.0f,-1.0f,-1.0f,1.0f };

		//アンマップ
		vertexBuff_->Unmap(0, nullptr);

	}

	//インデックスバッファ
	{

		indexBuff_ = CreateBufferResource(device_, sizeof(uint32_t) * 36);

		//インデックスバッファビュー設定
		ibView_.BufferLocation = indexBuff_->GetGPUVirtualAddress();
		ibView_.SizeInBytes = sizeof(uint32_t) * 36;
		ibView_.Format = DXGI_FORMAT_R32_UINT;

		//マッピングしてデータ転送
		indexBuff_->Map(0, nullptr, reinterpret_cast<void**>(&indexMap_));

		//各面のインデックスを設定
		for (int32_t i = 0; i < 6; i++) {

			indexMap_[i * 6 + 0] = 0 + i * 4;
			indexMap_[i * 6 + 1] = 1 + i * 4;
			indexMap_[i * 6 + 2] = 2 + i * 4;
			indexMap_[i * 6 + 3] = 2 + i * 4;
			indexMap_[i * 6 + 4] = 1 + i * 4;
			indexMap_[i * 6 + 5] = 3 + i * 4;

		}

		//アンマップ
		indexBuff_->Unmap(0, nullptr);

	}

	//transformMatrix
	{

		transformationBuff_ = CreateBufferResource(device_, sizeof(SkyboxTransformationMatrix));

		transformationBuff_->SetName(L"transformationBuff");

		transformationBuff_->Map(0, nullptr, reinterpret_cast<void**>(&transformationMap_));

		transformationMap_->WVP = MakeIdentity4x4();

		transformationBuff_->Unmap(0, nullptr);

	}

	//マテリアル
	{

		materialBuff_ = CreateBufferResource(device_, sizeof(SkyboxMaterial));

		materialBuff_->Map(0, nullptr, reinterpret_cast<void**>(&materialMap_));

		materialMap_->color = { 1.0f,1.0f,1.0f,1.0f };

		materialBuff_->Unmap(0, nullptr);

	}

}

void Skybox::LoadDss(const std::string& dssFileName) {

	tex_ = TextureManager::GetInstance()->Load(dssFileName);

}

void Skybox::Draw(Camera* camera) {

	Matrix4x4 worldViewProjectionMatrix = worldMatrix_ * camera->matViewProjection_;
	transformationMap_->WVP = worldViewProjectionMatrix;

	//ルートシグネチャを設定
	commandList_->SetGraphicsRootSignature(rootSignature_);
	//プリミティブ形状を設定
	commandList_->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	commandList_->SetGraphicsRootConstantBufferView(0, materialBuff_->GetGPUVirtualAddress());

	commandList_->SetGraphicsRootConstantBufferView(1, transformationBuff_->GetGPUVirtualAddress());

	commandList_->SetGraphicsRootDescriptorTable(2, tex_->srvHandleGPU);

	//頂点バッファビューの設定
	commandList_->IASetVertexBuffers(0, 1, &vbView_);
	//インデックスバッファビューの設定
	commandList_->IASetIndexBuffer(&ibView_);

	//描画
	commandList_->DrawIndexedInstanced(36, 1, 0, 0, 0);

}
