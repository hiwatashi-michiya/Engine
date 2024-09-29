#pragma once
#include "Sprite.h"
#include "TextureManager.h"
#include <memory>
#include <string>
#include "PostEffectDrawer.h"

class SceneChangeManager
{
public:

	static SceneChangeManager* GetInstance();

	void Initialize();

	void SceneChangeStart();

	void Update();

	void Draw();

	void SetNextScene(const std::string& sceneName) { nextScene_ = sceneName; }

	bool GetIsSceneChangeEnter() const { return isSceneChangeEnter_; }

	bool GetIsSceneChangeExit() const { return isSceneChangeExit_; }

	//シーン切り替え中かどうか
	bool IsSceneChange() { return isSceneChangeEnter_ || isSceneChangeExit_; }

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
