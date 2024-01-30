#include "PipelineManager.h"
#include <cassert>
#include "Convert.h"

PipelineManager* PipelineManager::GetInstance() {
	static PipelineManager instance;
	return &instance;
}

void PipelineManager::Initialize(ID3D12Device* device) {

	assert(device);

	device_ = device;

}

void PipelineManager::CreatePipeLine(const std::string& shaderFileName, ShaderManager::ShaderType type,
	BlendMode blendMode, const std::string& pipelineName) {



}
