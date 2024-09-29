#include "RenderTexture.h"
#include "PipelineManager.h"
#include "RootSignatureManager.h"
#include "ShaderManager.h"
#include "Core/DirectXSetter.h"
#include <cassert>
#include "Convert.h"

RenderTexture::RenderTexture()
{
}

RenderTexture::~RenderTexture()
{
}

void RenderTexture::Create(Microsoft::WRL::ComPtr<ID3D12Device> device, int32_t width, int32_t height,
	DXGI_FORMAT format, const Vector4& clearColor) {

	//生成するResourceの設定
	D3D12_RESOURCE_DESC resourceDesc{};
	resourceDesc.Width = width; //Textureの幅
	resourceDesc.Height = height; //Textureの高さ
	resourceDesc.MipLevels = 1; //mipmapの数
	resourceDesc.DepthOrArraySize = 1; //奥行き or 配列Textureの配列数
	resourceDesc.Format = format; //DepthStencilとして利用可能なフォーマット
	resourceDesc.SampleDesc.Count = 1; //サンプリングカウント。1固定
	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D; //2次元
	resourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET; //RenderTargetとして利用

	//利用するHeapの設定
	D3D12_HEAP_PROPERTIES heapProperties{};
	heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT; //VRAM上に作る

	//深度値のクリア設定
	D3D12_CLEAR_VALUE clearValue{};
	clearValue.Format = format;
	clearValue.Color[0] = clearColor.x;
	clearValue.Color[1] = clearColor.y;
	clearValue.Color[2] = clearColor.z;
	clearValue.Color[3] = clearColor.w;

	//Resourceの生成
	HRESULT hr = device->CreateCommittedResource(
		&heapProperties, //Heapの設定
		D3D12_HEAP_FLAG_NONE, //Heapの特殊な設定。特になし
		&resourceDesc, //Resourceの設定
		D3D12_RESOURCE_STATE_RENDER_TARGET, //深度値を書き込む状態にしておく
		&clearValue, //Clear最適値
		IID_PPV_ARGS(&renderTexture_)); //作成するResourceポインタへのポインタ
	assert(SUCCEEDED(hr));

	renderTexture_->SetName(L"renderTexture");

}

void RenderTexture::Draw() {

	ID3D12GraphicsCommandList* commandList = DirectXSetter::GetInstance()->GetCommandList();

	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	commandList->SetGraphicsRootDescriptorTable(0, srvHandleGPU_);

	commandList->DrawInstanced(3, 1, 0, 0);

}

void RenderTexture::Draw(ID3D12RootSignature* rootSignature, ID3D12PipelineState* pipelineState) {

	ID3D12GraphicsCommandList* commandList = DirectXSetter::GetInstance()->GetCommandList();

	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	commandList->SetGraphicsRootSignature(rootSignature);
	commandList->SetPipelineState(pipelineState);
	commandList->SetGraphicsRootDescriptorTable(0, srvHandleGPU_);

	commandList->DrawInstanced(3, 1, 0, 0);

}
