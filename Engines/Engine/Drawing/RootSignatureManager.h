#pragma once
#include <wrl.h>
#include <d3d12.h>
#include <string>
#include <unordered_map>

class RootSignatureManager
{
public:
	
	static RootSignatureManager* GetInstance();

	void Initialize(ID3D12Device* device);

	void CreateRootSignature(Microsoft::WRL::ComPtr<ID3DBlob> signatureBlob, const std::string& name);

	ID3D12RootSignature* GetRootSignature(const std::string& name);

private:

	ID3D12Device* device_;

	std::unordered_map<std::string, Microsoft::WRL::ComPtr<ID3D12RootSignature>> rootSignatures_;

private:

	RootSignatureManager() = default;
	~RootSignatureManager() = default;
	RootSignatureManager(const RootSignatureManager&) = delete;
	const RootSignatureManager& operator=(const RootSignatureManager&) = delete;

};


