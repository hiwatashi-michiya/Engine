#pragma once
#include <array>
#include <d3d12.h>
#include <string>
#include <wrl.h>
#include <windows.h>

struct Texture {

	//リソース
	Microsoft::WRL::ComPtr<ID3D12Resource> resource;
	//CPUハンドル
	D3D12_CPU_DESCRIPTOR_HANDLE srvHandleCPU;
	//GPUハンドル
	D3D12_GPU_DESCRIPTOR_HANDLE srvHandleGPU;

};

class TextureManager
{
public:

	static const size_t kNumDescriptors = 256;

	static TextureManager* GetInstance();

	void Initialize();

	Texture Load(const std::string& filePath);

	void Finalize();

	ID3D12DescriptorHeap* GetSRVDescHeap() { return srvDescHeap_.Get(); }

private:

	TextureManager() = default;
	~TextureManager() = default;
	TextureManager(const TextureManager&) = delete;
	const TextureManager& operator=(const TextureManager&) = delete;

private:

	uint32_t descriptorSizeSRV_;

	ID3D12Device* device_;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> srvDescHeap_;
	std::array<Texture, kNumDescriptors> textures_;

	//ロードしたテクスチャの数
	uint32_t textureIndex_ = 0;

	const uint32_t kMaxTextures = 256;

};

