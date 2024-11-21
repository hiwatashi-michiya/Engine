#include "Sprite.h"
#include <cassert>
#include "Convert.h"
#include "Drawing/ShaderManager.h"
#include "Drawing/PipelineManager.h"
#include "Drawing/RootSignatureManager.h"
#include "Drawing/ImGuiManager.h"
#include "Buffer/BufferResource.h"
#include "Drawing/RenderManager.h"

#pragma comment(lib, "dxcompiler.lib")

ID3D12Device* Sprite::device_ = nullptr;
ID3D12GraphicsCommandList* Sprite::commandList_ = nullptr;
ID3D12RootSignature* Sprite::rootSignature2D_ = nullptr;
ID3D12PipelineState* Sprite::pipelineState2D_ = nullptr;
IDxcBlob* Sprite::vs2dBlob_ = nullptr;
IDxcBlob* Sprite::ps2dBlob_ = nullptr;

//静的初期化
void Sprite::StaticInitialize(ID3D12Device* device, int window_width,
	int window_height) {

	assert(device);

	HRESULT hr;

	device_ = device;

	//Shaderをコンパイルする
	vs2dBlob_ = ShaderManager::GetInstance()->CompileShader(L"./resources/shaders/Sprite.VS.hlsl", ShaderManager::kVS, "VS2D");
		
	ps2dBlob_ = ShaderManager::GetInstance()->CompileShader(L"./resources/shaders/Sprite.PS.hlsl", ShaderManager::kPS, "PS2D");

	//頂点レイアウト
	D3D12_INPUT_ELEMENT_DESC inputElementDescs[2] = {};
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

	//ルートパラメータ作成
	D3D12_ROOT_PARAMETER rootParameters[3]{};
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

	RootSignatureManager::GetInstance()->CreateRootSignature(signatureBlob, "RootSignature2D");

	rootSignature2D_ = RootSignatureManager::GetInstance()->GetRootSignature("RootSignature2D");

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
	graphicsPipelineStateDesc.pRootSignature = rootSignature2D_; //RootSignature
	graphicsPipelineStateDesc.InputLayout = inputLayoutDesc; //InputLayout
	graphicsPipelineStateDesc.VS = { vs2dBlob_->GetBufferPointer(),
	vs2dBlob_->GetBufferSize() }; //VertexShader
	graphicsPipelineStateDesc.PS = { ps2dBlob_->GetBufferPointer(),
	ps2dBlob_->GetBufferSize() }; //PixelShader
	graphicsPipelineStateDesc.BlendState = blendDesc; //BlendState
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
	depthStencilDesc.DepthEnable = false;
	//書き込み
	depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	//近ければ描画
	depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;

	//DepthStencilの設定
	graphicsPipelineStateDesc.DepthStencilState = depthStencilDesc;
	graphicsPipelineStateDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;

	PipelineManager::GetInstance()->CreatePipeLine(graphicsPipelineStateDesc, "PipelineNormal2D");

	pipelineState2D_ = PipelineManager::GetInstance()->GetPipeline("PipelineNormal2D");

}

Sprite::Sprite(Texture* texture, Vector2 position, Vector2 size, Vector4 color) {
	texture_ = texture;
	position_ = position;
	size_ = size;
	viewRect_ = { 1.0f,1.0f };
	color_ = color;
}

Sprite* Sprite::Create(Texture* texture, Vector2 position, Vector4 color) {

	Vector2 tmpSize = { 100.0f,100.0f };

	tmpSize = { static_cast<float>(texture->resource->GetDesc().Width),
	static_cast<float>(texture->resource->GetDesc().Height) };

	Sprite* sprite = new Sprite(texture, position, tmpSize, color);

	if (sprite == nullptr) {
		return nullptr;
	}

	if (!sprite->Initialize()) {
		delete sprite;
		assert(0);
		return nullptr;
	}

	return sprite;

}

bool Sprite::Initialize() {

	assert(device_);

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
		vertMap_[0].position = { 0.0f,size_.y, 0.0f,1.0f };
		vertMap_[0].texcoord = { 0.0f,1.0f };
		//左上
		vertMap_[1].position = { 0.0f,0.0f, 0.0f,1.0f };
		vertMap_[1].texcoord = { 0.0f,0.0f };
		//右下
		vertMap_[2].position = { size_.x,size_.y, 0.0f,1.0f };
		vertMap_[2].texcoord = { 1.0f,1.0f };
		//右上
		vertMap_[3].position = { size_.x,0.0f, 0.0f,1.0f };
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

		constMap_->color = color_;
		constMap_->uvTransform = MakeIdentity4x4();

		//アンマップ
		constBuff_->Unmap(0, nullptr);

	}

	//transformMatrix
	{

		matBuff_ = CreateBufferResource(device_, sizeof(Matrix4x4));

		matBuff_->Map(0, nullptr, reinterpret_cast<void**>(&matTransformMap_));

		*matTransformMap_ = MakeIdentity4x4();

		matBuff_->Unmap(0, nullptr);

	}

	return true;

}

void Sprite::PreDraw(ID3D12GraphicsCommandList* commandList) {

	assert(commandList_ == nullptr);

	commandList_ = commandList;

	//PSO設定
	commandList_->SetPipelineState(pipelineState2D_);
	//ルートシグネチャを設定
	commandList_->SetGraphicsRootSignature(rootSignature2D_);
	//プリミティブ形状を設定
	commandList_->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

}

void Sprite::PostDraw() {
	//コマンドリスト解除
	commandList_ = nullptr;

}

void Sprite::Draw() {

	//左下
	vertMap_[0].position = { 0.0f,size_.y, 0.0f,1.0f };
	vertMap_[0].texcoord = { 0.0f,viewRect_.y };
	//左上
	vertMap_[1].position = { 0.0f,0.0f, 0.0f,1.0f };
	vertMap_[1].texcoord = { 0.0f,0.0f };
	//右下
	vertMap_[2].position = { size_.x,size_.y, 0.0f,1.0f };
	vertMap_[2].texcoord = { viewRect_.x,viewRect_.y };
	//右上
	vertMap_[3].position = { size_.x,0.0f, 0.0f,1.0f };
	vertMap_[3].texcoord = { viewRect_.x,0.0f };

	Matrix4x4 worldMatrix = MakeAffineMatrix({ 1.0f,1.0f,1.0f }, { 0.0f,0.0f,0.0f }, { position_.x, position_.y, 0.5f });
	Matrix4x4 viewMatrix = MakeIdentity4x4();
	Matrix4x4 projectionMatrix = MakeOrthographicMatrix(0.0f, 0.0f, 1280.0f, 720.0f, 0.0f, 100.0f);
	Matrix4x4 worldViewProjectionMatrix = Multiply(worldMatrix, Multiply(viewMatrix, projectionMatrix));
	*matTransformMap_ = worldViewProjectionMatrix;

	constMap_->color = color_;

	Matrix4x4 matUVTransform = MakeScaleMatrix(Vector3(uvScale_.x, uvScale_.y, 1.0f)) * MakeRotateZMatrix(uvRotate_) *
		MakeTranslateMatrix(Vector3(uvTranslate_.x, uvTranslate_.y, 0.0f));

	constMap_->uvTransform = matUVTransform;

	RenderManager::GetInstance()->AddSprite(this);

}

void Sprite::Render()
{

	//Spriteの描画
	commandList_->IASetVertexBuffers(0, 1, &vbView_);
	commandList_->SetGraphicsRootConstantBufferView(1, matBuff_->GetGPUVirtualAddress());
	//IBVを設定
	commandList_->IASetIndexBuffer(&ibView_);
	////SRVの設定
	commandList_->SetGraphicsRootDescriptorTable(2, texture_->srvHandleGPU);
	commandList_->SetGraphicsRootConstantBufferView(0, constBuff_->GetGPUVirtualAddress());
	//描画
	commandList_->DrawIndexedInstanced(6, 1, 0, 0, 0);

}

void Sprite::Finalize() {

}

void Sprite::ImGuiUpdate(const std::string name) {

#ifdef _DEBUG

	ImGui::Begin(name.c_str());
	ImGui::DragFloat2("position", &position_.x, 0.1f);
	ImGui::DragFloat2("size", &size_.x, 0.01f);
	ImGui::DragFloat2("view rect", &viewRect_.x, 0.01f, 0.0f, 1.0f);
	ImGui::DragFloat2("uv scale", &uvScale_.x, 0.01f);
	ImGui::DragFloat("uv rotate", &uvRotate_, 0.01f);
	ImGui::DragFloat2("uv translate", &uvTranslate_.x, 0.01f);
	ImGui::End();

#endif // _DEBUG

}
