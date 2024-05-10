#pragma once

class SceneManager;

class BaseScene
{
public:
	
	virtual void Initialize() = 0;

	virtual void Update() = 0;

	virtual void DrawModel() = 0;

	virtual void DrawSkinningModel() = 0;

	virtual void DrawParticle() = 0;

	virtual void DrawSprite() = 0;

	virtual void DrawLine() = 0;

	virtual ~BaseScene() = default;

	virtual void SetSceneManager(SceneManager* sceneManager) { sceneManager_ = sceneManager; }

private:

	SceneManager* sceneManager_ = nullptr;

};
