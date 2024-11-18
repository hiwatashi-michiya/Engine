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
	
	static PostEffectDrawer* GetInstance();

	void Initialize();

	void Draw();

	void Debug();

	void SetType(PostEffectType type) { type_ = type; }

	void SetCamera(Camera* camera) { for (int32_t i = 0; i < postEffects_.size(); i++) { postEffects_[i]->SetCamera(camera); } }

	std::vector<std::shared_ptr<PostEffects>> GetPostEffects() const { return postEffects_; }

private:

	DirectXSetter* dxSetter_ = nullptr;

	ID3D12Device* device_ = nullptr;

	std::vector<std::shared_ptr<PostEffects>> postEffects_;

	std::vector<RenderTexture> renderTextures_;

	RenderTexture renderTexture_;

	PostEffectType type_ = kDepthBasedOutline;

	//最初に一回だけ描画
	bool isUsedAllEffects_ = false;

private:

	PostEffectDrawer() = default;
	~PostEffectDrawer() = default;
	PostEffectDrawer(const PostEffectDrawer&) = delete;
	const PostEffectDrawer& operator=(const PostEffectDrawer&) = delete;

};


