#include "RootParameter.h"

ML_RootParameter::ML_RootParameter()
{
}

ML_RootParameter::~ML_RootParameter()
{
}

void ML_RootParameter::SetRootParameter(D3D12_ROOT_PARAMETER_TYPE parameterType, D3D12_SHADER_VISIBILITY shaderVisibility, uint32_t shaderRegister, uint32_t rootParameterIndex)
{
	//サイズより大きい値なら早期リターン
	if (rootParameterIndex >= rootParameters_.size()) {
		return;
	}

	rootParameters_[rootParameterIndex].ParameterType = parameterType;
	rootParameters_[rootParameterIndex].ShaderVisibility = shaderVisibility;
	rootParameters_[rootParameterIndex].Descriptor.ShaderRegister = shaderRegister;

}

void ML_RootParameter::SetRootParameter(D3D12_ROOT_PARAMETER_TYPE parameterType, D3D12_SHADER_VISIBILITY shaderVisibility, const std::vector<D3D12_DESCRIPTOR_RANGE>& descriptorRange, uint32_t rootParameterIndex)
{
	//サイズより大きい値なら早期リターン
	if (rootParameterIndex >= rootParameters_.size()) {
		return;
	}

	rootParameters_[rootParameterIndex].ParameterType = parameterType;
	rootParameters_[rootParameterIndex].ShaderVisibility = shaderVisibility;
	rootParameters_[rootParameterIndex].DescriptorTable.pDescriptorRanges = descriptorRange.data();
	rootParameters_[rootParameterIndex].DescriptorTable.NumDescriptorRanges = UINT(descriptorRange.size());


}
