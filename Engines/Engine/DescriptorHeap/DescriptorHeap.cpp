#include "DescriptorHeap.h"
#include <cassert>
#include <Windows.h>

DescriptorHeap::DescriptorHeap() {

}

DescriptorHeap::~DescriptorHeap() {

}

void DescriptorHeap::Create(ID3D12Device* device, D3D12_DESCRIPTOR_HEAP_TYPE heapType,
	UINT numDescriptors, bool shaderVisible, const std::string& name) {

	//ディスクリプタヒープの生成
	D3D12_DESCRIPTOR_HEAP_DESC descriptorHeapDesc{};
	descriptorHeapDesc.Type = heapType;
	descriptorHeapDesc.NumDescriptors = numDescriptors;
	descriptorHeapDesc.Flags = shaderVisible ? D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE : D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	HRESULT hr = device->CreateDescriptorHeap(&descriptorHeapDesc, IID_PPV_ARGS(&heap_));
	//ディスクリプタヒープが作れなかったので起動できない
	if (!SUCCEEDED(hr)) {

		MessageBox(nullptr, L"Failed Create DescriptorHeap", L"DescriptorHeap::Create", 0);

		assert(false);

	}

	limit_ = numDescriptors;

}

const uint32_t& DescriptorHeap::GetCurrentIndex() {

	assert(index_ < limit_);

	return index_;

}

uint32_t DescriptorHeap::AddIndex() {

	assert(index_ < limit_);

	return index_++;

}
