#pragma once
#include <cmath>
#include <d3d12.h>
#include <wrl.h>

/// <summary>
/// 深度値を扱いやすくするためのクラス
/// </summary>
class DepthStencil
{
public:
	DepthStencil();
	~DepthStencil();
	//デプスステンシル生成
	void Create(ID3D12Device* device, int32_t width, int32_t height);

	ID3D12Resource* Get() { return depthStencil_.Get(); }

private:

	Microsoft::WRL::ComPtr<ID3D12Resource> depthStencil_;

};


