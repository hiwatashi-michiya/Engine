#pragma once

class SceneManager;

class BaseScene
{
public:
	
	virtual void Initialize() = 0;

	virtual void Update() = 0;

	virtual void Draw() = 0;

	virtual ~BaseScene() = default;

	virtual void SetSceneManager(SceneManager* sceneManager) { sceneManager_ = sceneManager; }

private:

	SceneManager* sceneManager_ = nullptr;

};
