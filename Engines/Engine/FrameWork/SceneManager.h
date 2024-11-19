#pragma once
#include "AbsSceneFactory.h"
#include "BaseScene.h"
#include <memory>

class SceneManager
{
public:
	
	static SceneManager* GetInstance();

	void Update();

	void Finalize();

	void Draw();

	void ChangeScene(const std::string& sceneName);

	void SetSceneFactory(AbsSceneFactory* sceneFactory) { sceneFactory_ = sceneFactory; }

private:

	AbsSceneFactory* sceneFactory_ = nullptr;

	std::unique_ptr<BaseScene> scene_;

	std::unique_ptr<BaseScene> nextScene_;

private:

	SceneManager() = default;
	~SceneManager() = default;
	SceneManager(const SceneManager&) = delete;
	const SceneManager& operator=(const SceneManager&) = delete;

};
