#pragma once
#include "RenderTexture.h"
#include "base/DirectXSetter.h"

/// <summary>
/// フルスクリーンポストエフェクト描画クラス
/// </summary>
class PostEffectDrawer
{
public:
	
	static PostEffectDrawer* GetInstance();

	void Initialize();

	void Draw();

	void SetType(PostEffectType type) { renderTexture_.type_ = type; }

private:

	DirectXSetter* dxSetter_ = nullptr;

	ID3D12Device* device_ = nullptr;

	RenderTexture renderTexture_;

private:

	PostEffectDrawer() = default;
	~PostEffectDrawer() = default;
	PostEffectDrawer(const PostEffectDrawer&) = delete;
	const PostEffectDrawer& operator=(const PostEffectDrawer&) = delete;

};


