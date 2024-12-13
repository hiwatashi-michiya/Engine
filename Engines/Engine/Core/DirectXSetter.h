#pragma once

#include "DepthStencil.h"
#include "RenderTexture.h"
#include "WindowManager.h"
#include <chrono>
#include <d3d12.h>
#include <dxcapi.h>
#include <dxgi1_6.h>
#include <dxgidebug.h>
#include <thread>
#include <vector>
#include <Windows.h>
#include <wrl.h>

/// <summary>
/// DirectX12を使用した初期化や描画処理関連の流れを形成するクラス
/// </summary>
class DirectXSetter
{

public:
	//インスタンス取得
	static DirectXSetter* GetInstance();

	//srvHeapのHandle
	static uint32_t srvHandleNumber_;

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

	/// <summary>
	/// RenderTextureに対しての描画前処理
	/// </summary>
	void RenderTexturePreDraw();

	/// <summary>
	/// RenderTextureに対しての描画後処理
	/// </summary>
	void RenderTexturePostDraw();

	/// <summary>
	/// 終了処理
	/// </summary>
	void Finalize();
	/// <summary>
	/// デバイス取得
	/// </summary>
	/// <returns></returns>
	ID3D12Device* GetDevice() { return device_.Get(); }
	/// <summary>
	/// コマンドリスト取得
	/// </summary>
	/// <returns></returns>
	ID3D12GraphicsCommandList* GetCommandList() { return commandList_.Get(); }
	/// <summary>
	/// SRVデスクリプタヒープ取得
	/// </summary>
	/// <returns></returns>
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> GetSrvHeap() { return srvHeap_; }
	/// <summary>
	/// RTVデスクリプタヒープ取得
	/// </summary>
	/// <returns></returns>
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> GetRtvHeap() { return rtvHeap_; }
	/// <summary>
	/// デプスステンシル取得
	/// </summary>
	/// <returns></returns>
	ID3D12Resource* GetDepthStencilResource() { return depthStencil_.Get(); }

private:

	WindowManager* windowManager_;

	//ファクトリー
	Microsoft::WRL::ComPtr<IDXGIFactory7> dxgiFactory_;
	//デバイス
	Microsoft::WRL::ComPtr<ID3D12Device> device_;
	//コマンドキュー
	Microsoft::WRL::ComPtr<ID3D12CommandQueue> commandQueue_;
	//アロケータ
	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> commandAllocator_;
	//コマンドリスト
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList_;
	//スワップチェーン
	Microsoft::WRL::ComPtr<IDXGISwapChain4> swapChain_;
	//スワップチェーンデスク
	DXGI_SWAP_CHAIN_DESC1 swapChainDesc{};
	//バックバッファ
	std::vector<Microsoft::WRL::ComPtr<ID3D12Resource>> backBuffers_;
	//デプスステンシル
	DepthStencil depthStencil_;
	//レンダーターゲットビューデスクリプタヒープ
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> rtvHeap_;
	//デプスステンシルビューデスクリプタヒープ
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> dsvHeap_;
	//シェーダーリソースビューデスクリプタヒープ
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> srvHeap_;
	//フェンス
	Microsoft::WRL::ComPtr<ID3D12Fence> fence_;
	//フェンスの値
	UINT64 fenceVal_ = 0;
	//横幅
	int32_t backBufferWidth_ = 0;
	//縦幅
	int32_t backBufferHeight_ = 0;

	//DescriptorSizeを取得しておく
	uint32_t descriptorSizeSRV;
	uint32_t descriptorSizeRTV;
	uint32_t descriptorSizeDSV;

	//記録時間(FPS固定用)
	std::chrono::steady_clock::time_point reference_;
	//前回参照時間
	std::chrono::steady_clock::time_point preReference_;

private:
	//シングルトン化
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
	void CreateRenderTargets();

	//深度バッファ生成
	void CreateDepthBuffer();

	//シェーダーリソースビューのヒープ生成
	void CreateSrvHeap();

	//フェンス生成
	void CreateFence();

	//FPS固定初期化
	void InitializeFixFPS();

	//FPS固定更新
	void UpdateFixFPS();

};

