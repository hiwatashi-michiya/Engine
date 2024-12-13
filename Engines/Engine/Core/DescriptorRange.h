#pragma once
#include <d3d12.h>
#include <vector>

/// <summary>
/// デスクリプタレンジを扱いやすくしたクラス
/// </summary>
class ML_DescriptorRange
{
public:
	ML_DescriptorRange();
	~ML_DescriptorRange();
	//サイズセット
	void SetSize(uint32_t size) { descriptorRanges_.resize(size); }
	//デスクリプタレンジセット
	void SetDescriptorRange(uint32_t baseShaderRegister, uint32_t numDescriptors,
		D3D12_DESCRIPTOR_RANGE_TYPE rangeType, uint32_t offset,
		uint32_t descriptorRangeIndex);
	//デスクリプタレンジの本体取得
	const std::vector<D3D12_DESCRIPTOR_RANGE>& Get() const { return descriptorRanges_; }

private:

	std::vector<D3D12_DESCRIPTOR_RANGE> descriptorRanges_;

};


