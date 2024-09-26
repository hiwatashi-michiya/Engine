#include "PostEffectDrawer.h"
#include <cassert>
#include "DescriptorHandle.h"
#include "ImGuiManager.h"

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

	DirectXSetter::srvHandleNumber_++;

	for (int32_t i = 0; i < 2; i++) {

		RenderTexture newRenderTex;

		//レンダーターゲットビューの設定
		D3D12_RENDER_TARGET_VIEW_DESC renderTargetViewDesc{};
		renderTargetViewDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
		renderTargetViewDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;

		//RenderTexture用のRTVを作成
		const Vector4 kRenderTargetClearValue{ 0.05f,0.1f,0.5f,1.0f}; //青

		//ディスクリプタヒープのハンドルを取得
		D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = GetCPUDescriptorHandle(dxSetter_->GetRtvHeap().Get(), descriptorSizeRTV, 2 + i);
		newRenderTex.Create(device_, WindowManager::kWindowWidth, WindowManager::kWindowHeight,
			DXGI_FORMAT_R8G8B8A8_UNORM_SRGB, kRenderTargetClearValue);
		device_->CreateRenderTargetView(newRenderTex.Get(), &renderTargetViewDesc, rtvHandle);

		//SRVの設定。FormatはResourceと同じ
		D3D12_SHADER_RESOURCE_VIEW_DESC renderTextureSrvDesc{};
		renderTextureSrvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
		renderTextureSrvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		renderTextureSrvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;//2Dテクスチャ
		renderTextureSrvDesc.Texture2D.MipLevels = 1;

		D3D12_CPU_DESCRIPTOR_HANDLE srvHandleCPU =
			GetCPUDescriptorHandle(dxSetter_->GetSrvHeap().Get(), descriptorSizeSRV, DirectXSetter::srvHandleNumber_);

		D3D12_GPU_DESCRIPTOR_HANDLE srvHandleGPU =
			GetGPUDescriptorHandle(dxSetter_->GetSrvHeap().Get(), descriptorSizeSRV, DirectXSetter::srvHandleNumber_);

		DirectXSetter::srvHandleNumber_++;

		newRenderTex.SetCPUHandle(srvHandleCPU);
		newRenderTex.SetGPUHandle(srvHandleGPU);

		device_->CreateShaderResourceView(newRenderTex.Get(), &renderTextureSrvDesc, srvHandleCPU);

		renderTextures_.push_back(newRenderTex);

	}

	postEffects_.push_back(std::make_shared<CopyImageRender>());

	postEffects_.push_back(std::make_shared<Grayscale>());

	postEffects_.push_back(std::make_shared<Vignette>());

	postEffects_.push_back(std::make_shared<BoxFilter>());

	postEffects_.push_back(std::make_shared<GaussianFilter>());

	postEffects_.push_back(std::make_shared<LuminanceBasedOutline>());

	postEffects_.push_back(std::make_shared<DepthBasedOutline>());

	postEffects_.push_back(std::make_shared<RadialBlur>());

	postEffects_.push_back(std::make_shared<HSVFilter>());

	postEffects_.push_back(std::make_shared<Dissolve>());

	for (int32_t i = 0; i < postEffects_.size(); i++) {

		postEffects_[i]->Create();

	}

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
	barrier.Transition.pResource = renderTextures_[0].Get();
	//遷移前(現在)のResourceState
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
	//遷移後のResourceState
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
	//TransitionBarrierを張る
	commandList->ResourceBarrier(1, &barrier);

	//最初の一回だけ全部描画する
	if (!isUsedAllEffects_) {

		for (int32_t i = 0; i < postEffects_.size(); i++) {
			postEffects_[i]->Render();
			postEffects_[i]->PostRender();
		}

		isUsedAllEffects_ = true;

	}

	if (type_ < postEffects_.size()) {
		postEffects_[type_]->Render();
	}

	renderTextures_[0].Draw();

	if (type_ < postEffects_.size()) {
		postEffects_[type_]->PostRender();
	}

	//今回のバリアはTransition
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	//Noneにしておく
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	//バリアを張る対象のリソース。現在のバックバッファに対して行う
	barrier.Transition.pResource = renderTextures_[0].Get();
	//遷移前(現在)のResourceState
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
	//遷移後のResourceState
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
	//TransitionBarrierを張る
	commandList->ResourceBarrier(1, &barrier);

}

void PostEffectDrawer::Debug() {

#ifdef _DEBUG

	ImGui::Begin("PostEffects", nullptr, ImGuiWindowFlags_MenuBar);

	if (ImGui::Button("Normal")) {
		type_ = kNone;
	}

	if (ImGui::Button("GrayScale")) {
		type_ = kGrayscale;
	}

	if (ImGui::Button("Vignette")) {
		type_ = kVignette;
	}

	if (ImGui::Button("BoxFilter")) {
		type_ = kBoxFilter;
	}

	if (ImGui::Button("GaussianFilter")) {
		type_ = kGaussianFilter;
	}

	if (ImGui::BeginTabBar("PostEffectsItem")) {

		for (int32_t i = 0; i < postEffects_.size(); i++) {

			if (ImGui::BeginTabItem(postEffects_[i]->name_.c_str())) {

				postEffects_[i]->Debug();

				ImGui::EndTabItem();

			}

		}

		ImGui::EndTabBar();

	}

	ImGui::End();

#endif // _DEBUG

}
