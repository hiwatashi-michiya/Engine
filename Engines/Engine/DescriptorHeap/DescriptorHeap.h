#pragma once
#include <d3d12.h>
#include <string>
#include <unordered_map>
#include <wrl.h>

class DescriptorHeap
{
public:
	DescriptorHeap();
	~DescriptorHeap();

	void Create(ID3D12Device* device, D3D12_DESCRIPTOR_HEAP_TYPE heapType,
		UINT numDescriptors, bool shaderVisible, const std::string& name);

	//現在の使用数を返す
	const uint32_t& GetCurrentIndex();
	//現在の使用数を返し、使用数をインクリメントする
	uint32_t AddIndex();

	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> heap_;

private:

	//デスクリプタヒープの使用数
	uint32_t index_ = 0;
	//デスクリプタヒープの最大数
	uint32_t limit_ = 0;

};
