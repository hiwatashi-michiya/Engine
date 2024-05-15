#pragma once
#include <cmath>
#include <d3d12.h>
#include <wrl.h>

class DepthStencil
{
public:
	DepthStencil();
	~DepthStencil();

	void Create(ID3D12Device* device, int32_t width, int32_t height);

	ID3D12Resource* Get() { return depthStencil_.Get(); }

private:

	Microsoft::WRL::ComPtr<ID3D12Resource> depthStencil_;

};


