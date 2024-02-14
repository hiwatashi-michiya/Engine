#include "DescriptorHeapManager.h"
#include <cassert>

DescriptorHeapManager* DescriptorHeapManager::GetInstance() {
	static DescriptorHeapManager instance;
	return &instance;
}

void DescriptorHeapManager::Initialize(ID3D12Device* device) {

	assert(device);

	device_ = device;

}

//デスクリプタヒープ作成関数
ID3D12DescriptorHeap* DescriptorHeapManager::CreateDescriptorHeap(
	D3D12_DESCRIPTOR_HEAP_TYPE heapType, UINT numDescriptors, bool shaderVisible, const std::string& name) {

	if (descriptorHeaps_.find(name) != descriptorHeaps_.end()) {
		return descriptorHeaps_[name].Get();
	}

	//ディスクリプタヒープの生成
	D3D12_DESCRIPTOR_HEAP_DESC descriptorHeapDesc{};
	descriptorHeapDesc.Type = heapType;
	descriptorHeapDesc.NumDescriptors = numDescriptors;
	descriptorHeapDesc.Flags = shaderVisible ? D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE : D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	HRESULT hr = device_->CreateDescriptorHeap(&descriptorHeapDesc, IID_PPV_ARGS(&descriptorHeaps_[name]));
	//ディスクリプタヒープが作れなかったので起動できない
	assert(SUCCEEDED(hr));

	return descriptorHeaps_[name].Get();

}
