#pragma once
#include <d3d12.h>
#include <wrl.h>
#include <dxcapi.h>
#include <format>

Microsoft::WRL::ComPtr<ID3D12Resource> CreateBufferResource(Microsoft::WRL::ComPtr<ID3D12Device> device, size_t sizeInBytes);
