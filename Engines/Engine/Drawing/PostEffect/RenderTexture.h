#pragma once
#include "TextureManager.h"
#include "Vector4.h"
#include <array>
#include <cmath>
#include <d3d12.h>
#include <dxcapi.h>
#include <wrl.h>

class RenderTexture
{
public:
	RenderTexture();
	~RenderTexture();

	void Create(Microsoft::WRL::ComPtr<ID3D12Device> device, int32_t width, int32_t height,
		DXGI_FORMAT format, const Vector4& clearColor);

	void Draw();

	void Draw(ID3D12RootSignature* rootSignature, ID3D12PipelineState* pipelineState);

	ID3D12Resource* Get() { return renderTexture_.Get(); }

	void SetCPUHandle(D3D12_CPU_DESCRIPTOR_HANDLE& handle) {
		srvHandleCPU_ = handle;
	}

	void SetGPUHandle(D3D12_GPU_DESCRIPTOR_HANDLE& handle) {
		srvHandleGPU_ = handle;
	}
	
private:

	Microsoft::WRL::ComPtr<ID3D12Resource> renderTexture_;

	//CPUハンドル
	D3D12_CPU_DESCRIPTOR_HANDLE srvHandleCPU_;
	//GPUハンドル
	D3D12_GPU_DESCRIPTOR_HANDLE srvHandleGPU_;

};
