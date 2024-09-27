#pragma once
#include <d3d12.h>
#include <vector>

class ML_RootSignatureDesc
{
public:
	ML_RootSignatureDesc();
	~ML_RootSignatureDesc();

	void SetFlags(D3D12_ROOT_SIGNATURE_FLAGS flag);

	void SetRootParameter(const std::vector<D3D12_ROOT_PARAMETER>& rootParameters);

	void SetSamplers(const std::vector<D3D12_STATIC_SAMPLER_DESC>& samplers);

	const D3D12_ROOT_SIGNATURE_DESC& Get() const { return rootSignatureDesc_; }

private:

	D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc_{};

};


