#pragma once
#include <d3d12.h>
#include <vector>

/// <summary>
/// ルートパラメータを扱いやすくしたクラス
/// </summary>
class ML_RootParameter
{
public:
	ML_RootParameter();
	~ML_RootParameter();
	//サイズセット
	void SetSize(uint32_t size) { rootParameters_.resize(size); }
	//ルートパラメータセット
	void SetRootParameter(D3D12_ROOT_PARAMETER_TYPE parameterType,
		D3D12_SHADER_VISIBILITY shaderVisibility, uint32_t shaderRegister,
		uint32_t rootParameterIndex);
	//ルートパラメータセット(デスクリプタレンジ)
	void SetRootParameter(D3D12_ROOT_PARAMETER_TYPE parameterType,
		D3D12_SHADER_VISIBILITY shaderVisibility, const std::vector<D3D12_DESCRIPTOR_RANGE>& descriptorRange,
		uint32_t rootParameterIndex);
	//ルートパラメータの本体取得
	const std::vector<D3D12_ROOT_PARAMETER>& Get() const { return rootParameters_; }

private:

	std::vector<D3D12_ROOT_PARAMETER> rootParameters_;

};


