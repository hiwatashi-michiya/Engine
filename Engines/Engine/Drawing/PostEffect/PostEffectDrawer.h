#pragma once
#include "Core/DirectXSetter.h"
#include "RenderTexture.h"
#include <vector>
#include "PostEffects.h"
#include <memory>
#include "Camera.h"

/// <summary>
/// フルスクリーンポストエフェクト描画クラス
/// </summary>
class PostEffectDrawer
{
public:
	//インスタンス取得
	static PostEffectDrawer* GetInstance();
	//初期化
	void Initialize();
	//描画コマンド積む
	void Draw();
	//デバッグ
	void Debug();
	//エフェクトタイプセット
	void SetType(PostEffectType type) { type_ = type; }
	//カメラセット
	void SetCamera(Camera* camera) { for (int32_t i = 0; i < postEffects_.size(); i++) { postEffects_[i]->SetCamera(camera); } }
	//ポストエフェクトリスト取得
	std::vector<std::shared_ptr<PostEffects>> GetPostEffects() const { return postEffects_; }

private:

	DirectXSetter* dxSetter_ = nullptr;

	ID3D12Device* device_ = nullptr;
	//ポストエフェクトリスト
	std::vector<std::shared_ptr<PostEffects>> postEffects_;
	//レンダーテクスチャリスト
	std::vector<RenderTexture> renderTextures_;
	//レンダーテクスチャ
	RenderTexture renderTexture_;
	//ポストエフェクトのタイプ
	PostEffectType type_ = kDepthBasedOutline;

	//最初に一回だけ描画
	bool isUsedAllEffects_ = false;

private:
	//シングルトン化
	PostEffectDrawer() = default;
	~PostEffectDrawer() = default;
	PostEffectDrawer(const PostEffectDrawer&) = delete;
	const PostEffectDrawer& operator=(const PostEffectDrawer&) = delete;

};


