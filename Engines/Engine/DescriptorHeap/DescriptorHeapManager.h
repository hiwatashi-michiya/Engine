#pragma once
#include <d3d12.h>
#include <string>
#include <unordered_map>
#include <wrl.h>

/// <summary>
/// デスクリプタヒープ管理クラス
/// </summary>
class DescriptorHeapManager
{
public:
	//インスタンス取得
	static DescriptorHeapManager* GetInstance();
	//初期化
	void Initialize(ID3D12Device* device);

	//デスクリプタヒープ生成
	ID3D12DescriptorHeap* CreateDescriptorHeap(
		D3D12_DESCRIPTOR_HEAP_TYPE heapType, UINT numDescriptors, bool shaderVisible, const std::string& name);

private:

	ID3D12Device* device_ = nullptr;
	//デスクリプタヒープと名前を紐づけたマップ
	std::unordered_map<std::string, Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>> descriptorHeaps_;

private:
	//シングルトン化
	DescriptorHeapManager() = default;
	~DescriptorHeapManager() = default;
	DescriptorHeapManager(const DescriptorHeapManager&) = delete;
	const DescriptorHeapManager& operator=(const DescriptorHeapManager&) = delete;

};
