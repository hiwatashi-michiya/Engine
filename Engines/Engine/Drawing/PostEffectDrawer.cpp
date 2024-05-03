#include "PostEffectDrawer.h"
#include <cassert>
#include "base/DescriptorHandle.h"

PostEffectDrawer* PostEffectDrawer::GetInstance() {
	static PostEffectDrawer instance;
	return &instance;
}

void PostEffectDrawer::Initialize() {

	dxSetter_ = DirectXSetter::GetInstance();

	device_ = dxSetter_->GetDevice();

	//DescriptorSizeを取得しておく
	uint32_t descriptorSizeSRV = device_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	uint32_t descriptorSizeRTV = device_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	uint32_t descriptorSizeDSV = device_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);

	//レンダーターゲットビューの設定
	D3D12_RENDER_TARGET_VIEW_DESC renderTargetViewDesc{};
	renderTargetViewDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	renderTargetViewDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;

	//RenderTexture用のRTVを作成
	const Vector4 kRenderTargetClearValue{ 1.0f,0.0f,0.0f,1.0f }; //分かりやすい様に赤
	//ディスクリプタヒープのハンドルを取得
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = GetCPUDescriptorHandle(dxSetter_->GetRtvHeap().Get(), descriptorSizeRTV, 2);
	renderTexture_.Create(device_, WindowManager::kWindowWidth, WindowManager::kWindowHeight,
		DXGI_FORMAT_R8G8B8A8_UNORM_SRGB, kRenderTargetClearValue);
	device_->CreateRenderTargetView(renderTexture_.Get(), &renderTargetViewDesc, rtvHandle);

	//SRVの設定。FormatはResourceと同じ
	D3D12_SHADER_RESOURCE_VIEW_DESC renderTextureSrvDesc{};
	renderTextureSrvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	renderTextureSrvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	renderTextureSrvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;//2Dテクスチャ
	renderTextureSrvDesc.Texture2D.MipLevels = 1;

	D3D12_CPU_DESCRIPTOR_HANDLE srvHandleCPU =
		GetCPUDescriptorHandle(dxSetter_->GetSrvHeap().Get(), descriptorSizeSRV, 1);

	D3D12_GPU_DESCRIPTOR_HANDLE srvHandleGPU =
		GetGPUDescriptorHandle(dxSetter_->GetSrvHeap().Get(), descriptorSizeSRV, 1);

	renderTexture_.SetCPUHandle(srvHandleCPU);
	renderTexture_.SetGPUHandle(srvHandleGPU);

	device_->CreateShaderResourceView(renderTexture_.Get(), &renderTextureSrvDesc, srvHandleCPU);


}

void PostEffectDrawer::Draw() {

	ID3D12GraphicsCommandList* commandList = DirectXSetter::GetInstance()->GetCommandList();

	//TransitionBarrierの設定
	D3D12_RESOURCE_BARRIER barrier{};
	//今回のバリアはTransition
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	//Noneにしておく
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	//バリアを張る対象のリソース。現在のバックバッファに対して行う
	barrier.Transition.pResource = renderTexture_.Get();
	//遷移前(現在)のResourceState
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
	//遷移後のResourceState
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
	//TransitionBarrierを張る
	commandList->ResourceBarrier(1, &barrier);

	renderTexture_.Draw();

	//今回のバリアはTransition
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	//Noneにしておく
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	//バリアを張る対象のリソース。現在のバックバッファに対して行う
	barrier.Transition.pResource = renderTexture_.Get();
	//遷移前(現在)のResourceState
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
	//遷移後のResourceState
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
	//TransitionBarrierを張る
	commandList->ResourceBarrier(1, &barrier);

}
