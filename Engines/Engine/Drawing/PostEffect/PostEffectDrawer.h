#pragma once
#include "DirectXSetter.h"
#include "RenderTexture.h"
#include <vector>
#include "PostEffects.h"

/// <summary>
/// フルスクリーンポストエフェクト描画クラス
/// </summary>
class PostEffectDrawer
{
public:
	
	static PostEffectDrawer* GetInstance();

	void Initialize();

	void Draw();

private:

	DirectXSetter* dxSetter_ = nullptr;

	ID3D12Device* device_ = nullptr;

	std::vector<PostEffects> postEffects_;

	RenderTexture renderTexture_;

private:

	PostEffectDrawer() = default;
	~PostEffectDrawer() = default;
	PostEffectDrawer(const PostEffectDrawer&) = delete;
	const PostEffectDrawer& operator=(const PostEffectDrawer&) = delete;

};


