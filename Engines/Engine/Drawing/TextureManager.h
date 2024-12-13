#pragma once
#include <array>
#include <d3d12.h>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>
#include <windows.h>
#include <wrl.h>

//テクスチャ構造体
struct Texture {

	//リソース
	Microsoft::WRL::ComPtr<ID3D12Resource> resource;
	//CPUハンドル
	D3D12_CPU_DESCRIPTOR_HANDLE srvHandleCPU;
	//GPUハンドル
	D3D12_GPU_DESCRIPTOR_HANDLE srvHandleGPU;

};

//インスタンシング用のリソース構造体。中身はテクスチャ構造体と同様
struct InstancingResource {

	//リソース
	Microsoft::WRL::ComPtr<ID3D12Resource> resource;
	//CPUハンドル
	D3D12_CPU_DESCRIPTOR_HANDLE srvHandleCPU;
	//GPUハンドル
	D3D12_GPU_DESCRIPTOR_HANDLE srvHandleGPU;

};

/// <summary>
/// テクスチャ管理クラス
/// </summary>
class TextureManager
{
public:
	//最大デスクリプタ数
	static const size_t kNumDescriptors = 256;
	//インスタンス取得
	static TextureManager* GetInstance();
	//初期化
	void Initialize(Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> srvHeap);
	//テクスチャ読み込み
	Texture* Load(const std::string& filePath);
	//インスタンシングリソースセット
	InstancingResource SetInstancingResource(uint32_t instanceCount, Microsoft::WRL::ComPtr<ID3D12Resource> mapResource);
	//終了処理
	void Finalize();
	//SRVデスクリプタヒープ取得
	ID3D12DescriptorHeap* GetSRVDescHeap() { return srvDescHeap_.Get(); }
	//テクスチャの番号取得
	const uint32_t& GetTextureIndex() { return textureIndex_; }
	//インデックス加算
	void AddIndex() { textureIndex_++; }

private:
	//シングルトン化
	TextureManager() = default;
	~TextureManager() = default;
	TextureManager(const TextureManager&) = delete;
	const TextureManager& operator=(const TextureManager&) = delete;

private:

	uint32_t descriptorSizeSRV_;

	ID3D12Device* device_;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> srvDescHeap_;
	std::array<Texture, kNumDescriptors> textures_;
	std::unordered_map<std::string, std::unique_ptr<Texture>> textureMap_;

	//intermediateResourceを保持しておくためのvector
	std::unordered_map<uint32_t, Microsoft::WRL::ComPtr<ID3D12Resource>> intermediateResources_;

	//ロードしたテクスチャの数
	uint32_t textureIndex_ = 0;

	//テクスチャの読み込み最大数
	const uint32_t kMaxTextures = 256;

};

