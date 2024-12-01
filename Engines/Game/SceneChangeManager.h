#pragma once
#include "Sprite.h"
#include "TextureManager.h"
#include <memory>
#include <string>
#include "PostEffectDrawer.h"

/// <summary>
/// シーン切り替え時の動きを管理するクラス
/// </summary>
class SceneChangeManager
{
public:

	static SceneChangeManager* GetInstance();
	//初期化
	void Initialize();
	//シーン切り替え開始
	void SceneChangeStart();
	//更新
	void Update();
	//描画
	void Draw();
	//次のシーンをセットする
	void SetNextScene(const std::string& sceneName) { nextScene_ = sceneName; }
	//シーン切り替え開始フラグ
	bool GetIsSceneChangeEnter() const { return isSceneChangeEnter_; }
	//シーン切り替え終了フラグ
	bool GetIsSceneChangeExit() const { return isSceneChangeExit_; }

	//シーン切り替え中かどうか
	bool IsSceneChange() { return isSceneChangeEnter_ or isSceneChangeExit_; }

private:

	PostEffectDrawer* postEffectDrawer_ = nullptr;

	std::unique_ptr<Sprite> sprite_;

	std::string nextScene_;

	bool isSceneChangeEnter_ = false;

	bool isSceneChangeExit_ = false;

	float sceneChangeValue_ = 0.0f;

	float sceneChangeSpeed_ = 0.05f;

	Texture* texture_ = nullptr;

private:

	SceneChangeManager() = default;
	~SceneChangeManager() = default;
	SceneChangeManager(const SceneChangeManager&) = delete;
	const SceneChangeManager& operator=(const SceneChangeManager&) = delete;

};
