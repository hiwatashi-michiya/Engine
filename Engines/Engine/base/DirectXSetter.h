#pragma once

#include <Windows.h>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <dxgidebug.h>
#include <dxcapi.h>
#include <wrl.h>
#include <vector>
#include <chrono>
#include <thread>
#include "WindowManager.h"

class DirectXSetter
{

public:

	static DirectXSetter* GetInstance();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(WindowManager* windowManager_, int32_t backBufferWidth = WindowManager::kWindowWidth,
		int32_t backBufferHeight = WindowManager::kWindowHeight);

	/// <summary>
	/// 描画前処理
	/// </summary>
	void PreDraw();

	/// <summary>
	/// 描画後処理
	/// </summary>
	void PostDraw();

	ID3D12Device* GetDevice() { return device_.Get(); }

	ID3D12GraphicsCommandList* GetCommandList() { return commandList_.Get(); }

	/// <summary>
	/// 終了処理
	/// </summary>
	void Finalize();

private:

	WindowManager* windowManager_;

	Microsoft::WRL::ComPtr<IDXGIFactory7> dxgiFactory_;
	Microsoft::WRL::ComPtr<ID3D12Device> device_;
	Microsoft::WRL::ComPtr<ID3D12CommandQueue> commandQueue_;
	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> commandAllocator_;
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList_;
	Microsoft::WRL::ComPtr<IDXGISwapChain4> swapChain_;
	DXGI_SWAP_CHAIN_DESC1 swapChainDesc{};
	std::vector<Microsoft::WRL::ComPtr<ID3D12Resource>> backBuffers_;
	Microsoft::WRL::ComPtr<ID3D12Resource> depthStencilResource_;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> rtvHeap_;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> dsvHeap_;
	Microsoft::WRL::ComPtr<ID3D12Fence> fence_;
	UINT64 fenceVal_ = 0;
	int32_t backBufferWidth_ = 0;
	int32_t backBufferHeight_ = 0;

	//デスクリプタヒープ作成関数
	ID3D12DescriptorHeap* CreateDescriptorHeap(
		ID3D12Device* device, D3D12_DESCRIPTOR_HEAP_TYPE heapType, UINT numDescriptors, bool shaderVisible);

	//CPUのDescriptorHandle取得
	D3D12_CPU_DESCRIPTOR_HANDLE GetCPUDescriptorHandle(ID3D12DescriptorHeap* descriptorHeap, uint32_t descriptorSize, uint32_t index);

	//DepthStencilTexture作成関数
	ID3D12Resource* CreateDepthStencilTextureResource(ID3D12Device* device, int32_t width, int32_t height);

	//DescriptorSizeを取得しておく
	uint32_t descriptorSizeSRV;
	uint32_t descriptorSizeRTV;
	uint32_t descriptorSizeDSV;

	//記録時間(FPS固定用)
	std::chrono::steady_clock::time_point reference_;

private:

	DirectXSetter() = default;
	~DirectXSetter() = default;
	DirectXSetter(const DirectXSetter&) = delete;
	const DirectXSetter& operator=(const DirectXSetter&) = delete;

private:

	//デバイスの初期化
	void InitializeDXGIDevice();

	//コマンド関連の初期化
	void InitializeCommand();

	//スワップチェーンの生成
	void CreateSwapChain();

	//レンダーターゲット生成
	void CreateFinalRenderTargets();

	//深度バッファ生成
	void CreateDepthBuffer();

	//フェンス生成
	void CreateFence();

	//FPS固定初期化
	void InitializeFixFPS();

	//FPS固定更新
	void UpdateFixFPS();

};

