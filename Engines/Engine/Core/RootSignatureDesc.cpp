#include "RootSignatureDesc.h"

ML_RootSignatureDesc::ML_RootSignatureDesc()
{
}

ML_RootSignatureDesc::~ML_RootSignatureDesc()
{
}

void ML_RootSignatureDesc::SetFlags(D3D12_ROOT_SIGNATURE_FLAGS flag)
{

	rootSignatureDesc_.Flags = flag;

}

void ML_RootSignatureDesc::SetRootParameter(const std::vector<D3D12_ROOT_PARAMETER>& rootParameters)
{

	rootSignatureDesc_.pParameters = rootParameters.data();
	rootSignatureDesc_.NumParameters = UINT(rootParameters.size());

}

void ML_RootSignatureDesc::SetSamplers(const std::vector<D3D12_STATIC_SAMPLER_DESC>& samplers)
{

	rootSignatureDesc_.pStaticSamplers = samplers.data();
	rootSignatureDesc_.NumStaticSamplers = UINT(samplers.size());

}
