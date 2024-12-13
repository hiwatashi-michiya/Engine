#pragma once
#include <d3d12.h>
#include <vector>

/// <summary>
/// サンプラーを使いやすくしたクラス
/// </summary>
class ML_StaticSampler
{
public:
	ML_StaticSampler();
	~ML_StaticSampler();
	//サイズセット
	void SetSize(uint32_t size) { staticSamplers_.resize(size); }
	//サンプラーセット
	void SetSampler(D3D12_FILTER filter, D3D12_TEXTURE_ADDRESS_MODE address,
		D3D12_COMPARISON_FUNC comparisonFunc, float maxLOD,
		uint32_t shaderRegister, D3D12_SHADER_VISIBILITY shaderVisibility, uint32_t samplerIndex);
	//サンプラー本体取得
	const std::vector<D3D12_STATIC_SAMPLER_DESC>& Get() const { return staticSamplers_; }

private:

	std::vector<D3D12_STATIC_SAMPLER_DESC> staticSamplers_;

};
