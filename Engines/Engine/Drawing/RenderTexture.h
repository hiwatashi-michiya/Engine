#pragma once
#include <d3d12.h>
#include <wrl.h>
#include <cmath>
#include <dxcapi.h>
#include "Vector4.h"
#include "TextureManager.h"

class RenderTextureSetup 
{
public:

	static RenderTextureSetup* GetInstance();

	void Initialize();

	ID3D12RootSignature* GetRootSignature() { return rootSignature_.Get(); }

	ID3D12PipelineState* GetPipelineState() { return pipelineState_.Get(); }

private:

	//ルートシグネチャ
	Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature_;
	//PSO
	Microsoft::WRL::ComPtr<ID3D12PipelineState> pipelineState_;

	Microsoft::WRL::ComPtr<IDxcBlob> vsBlob_;
	Microsoft::WRL::ComPtr<IDxcBlob> psBlob_;

private:

	RenderTextureSetup() = default;
	~RenderTextureSetup() = default;
	RenderTextureSetup(const RenderTextureSetup&) = delete;
	const RenderTextureSetup& operator=(const RenderTextureSetup&) = delete;

};

class RenderTexture
{
public:
	RenderTexture();
	~RenderTexture();

	void Create(Microsoft::WRL::ComPtr<ID3D12Device> device, int32_t width, int32_t height,
		DXGI_FORMAT format, const Vector4& clearColor);

	void Draw();

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