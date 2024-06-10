#pragma once
#include "DirectXSetter.h"
#include "RenderTexture.h"
#include <vector>
#include "PostEffects.h"
#include <memory>

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

private:

	DirectXSetter* dxSetter_ = nullptr;

	ID3D12Device* device_ = nullptr;

	std::vector<std::shared_ptr<PostEffects>> postEffects_;

	RenderTexture renderTexture_;

	PostEffectType type_ = kBoxFilter;

private:

	PostEffectDrawer() = default;
	~PostEffectDrawer() = default;
	PostEffectDrawer(const PostEffectDrawer&) = delete;
	const PostEffectDrawer& operator=(const PostEffectDrawer&) = delete;

};


