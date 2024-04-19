#include "RootSignatureManager.h"
#include <cassert>

RootSignatureManager* RootSignatureManager::GetInstance() {
	static RootSignatureManager instance;
	return &instance;
}

void RootSignatureManager::Initialize(Microsoft::WRL::ComPtr<ID3D12Device> device) {

	assert(device);

	device_ = device;

}

void RootSignatureManager::CreateRootSignature(Microsoft::WRL::ComPtr<ID3DBlob> signatureBlob, const std::string& name) {

	if (rootSignatures_.find(name) != rootSignatures_.end()) {

		return;

	}

	HRESULT hr;

	//バイナリを元に生成
	hr = device_->CreateRootSignature(0, signatureBlob->GetBufferPointer(),
		signatureBlob->GetBufferSize(), IID_PPV_ARGS(&rootSignatures_[name]));
	assert(SUCCEEDED(hr));


}

ID3D12RootSignature* RootSignatureManager::GetRootSignature(const std::string& name) {

	if (rootSignatures_.find(name) != rootSignatures_.end()) {

		return rootSignatures_[name].Get();

	}

	return nullptr;

}
