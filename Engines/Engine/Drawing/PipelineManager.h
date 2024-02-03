#pragma once
#include <wrl.h>
#include <d3d12.h>
#include <string>
#include <vector>
#include <dxcapi.h>
#include <memory>
#include <array>
#include <unordered_map>
#include "ShaderManager.h"

/// <summary>
/// パイプラインの管理を行うクラス
/// </summary>
class PipelineManager
{
public:

	enum BlendMode {
		kNormal,//通常
		kAdd,//加算
		kSubtract,//減算
		kMultiply,//乗算
		kScreen,//スクリーン

		kCountBlend
	};

	static PipelineManager* GetInstance();

	void Initialize(ID3D12Device* device);

	/// <summary>
	/// パイプライン作成
	/// </summary>
	/// <param name="shaderFileName">シェーダーファイル名</param>
	/// <param name="type">シェーダーのタイプ</param>
	/// <param name="blendMode">ブレンド設定</param>
	/// <param name="pipelineName">パイプラインに付ける名前</param>
	void CreatePipeLine(const D3D12_GRAPHICS_PIPELINE_STATE_DESC& desc, const std::string& pipelineName);

	ID3D12PipelineState* GetPipeline(const std::string& pipelineName);

private:

	ID3D12Device* device_ = nullptr;

	std::unordered_map<std::string, Microsoft::WRL::ComPtr<ID3D12PipelineState>> pipelineStates_;

private:

	PipelineManager() = default;
	~PipelineManager() = default;
	PipelineManager(const PipelineManager&) = delete;
	const PipelineManager& operator=(const PipelineManager&) = delete;

};


