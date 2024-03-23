#pragma once
#include <d3d12.h>
#include <wrl.h>
#include <string>
#include <unordered_map>

class DescriptorHeapManager
{
public:

	static DescriptorHeapManager* GetInstance();

	void Initialize(ID3D12Device* device);

	ID3D12DescriptorHeap* CreateDescriptorHeap(
		D3D12_DESCRIPTOR_HEAP_TYPE heapType, UINT numDescriptors, bool shaderVisible, const std::string& name);

private:

	ID3D12Device* device_ = nullptr;

	std::unordered_map<std::string, Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>> descriptorHeaps_;

private:

	DescriptorHeapManager() = default;
	~DescriptorHeapManager() = default;
	DescriptorHeapManager(const DescriptorHeapManager&) = delete;
	const DescriptorHeapManager& operator=(const DescriptorHeapManager&) = delete;

};
