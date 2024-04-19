#include "PipelineManager.h"
#include <cassert>
#include "Convert.h"

PipelineManager* PipelineManager::GetInstance() {
	static PipelineManager instance;
	return &instance;
}

void PipelineManager::Initialize(Microsoft::WRL::ComPtr<ID3D12Device> device) {

	assert(device);

	device_ = device;

}

void PipelineManager::CreatePipeLine(const D3D12_GRAPHICS_PIPELINE_STATE_DESC& desc, const std::string& pipelineName) {

	//既にある場合新しく作らない
	if (pipelineStates_.find(pipelineName) != pipelineStates_.end()) {

		return;

	}

	HRESULT hr;

	hr = device_->CreateGraphicsPipelineState(&desc, IID_PPV_ARGS(&pipelineStates_[pipelineName]));

	assert(SUCCEEDED(hr));

}

ID3D12PipelineState* PipelineManager::GetPipeline(const std::string& pipelineName) {

	if (pipelineStates_.find(pipelineName) != pipelineStates_.end()) {

		return pipelineStates_[pipelineName].Get();

	}

	return nullptr;

}
