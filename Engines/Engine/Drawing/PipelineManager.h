#pragma once
#include <array>
#include <d3d12.h>
#include <dxcapi.h>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>
#include <wrl.h>

/// <summary>
/// パイプラインの管理を行うクラス
/// </summary>
class PipelineManager
{
public:

	//ブレンドモード設定
	enum BlendMode {
		kNormal,//通常
		kAdd,//加算
		kSubtract,//減算
		kMultiply,//乗算
		kScreen,//スクリーン

		kCountBlend
	};
	//インスタンス取得
	static PipelineManager* GetInstance();
	//初期化
	void Initialize(ID3D12Device* device);

	/// <summary>
	/// パイプライン作成
	/// </summary>
	/// <param name="blendMode">ブレンド設定</param>
	/// <param name="pipelineName">パイプラインに付ける名前</param>
	void CreatePipeLine(const D3D12_GRAPHICS_PIPELINE_STATE_DESC& desc, const std::string& pipelineName);
	//パイプライン取得
	ID3D12PipelineState* GetPipeline(const std::string& pipelineName);

private:

	ID3D12Device* device_ = nullptr;
	//パイプラインと名前を紐づけたマップ
	std::unordered_map<std::string, Microsoft::WRL::ComPtr<ID3D12PipelineState>> pipelineStates_;

private:
	//シングルトン化
	PipelineManager() = default;
	~PipelineManager() = default;
	PipelineManager(const PipelineManager&) = delete;
	const PipelineManager& operator=(const PipelineManager&) = delete;

};


