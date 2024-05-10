#pragma once
#include "Engine/base/DirectXSetter.h"
#include "Engine/Drawing/TextureManager.h"
#include "Engine/3d/Model.h"
#include "Engine/input/Input.h"
#include "Engine/2d/Sprite.h"
#include <memory>
#include "Game/player/Player.h"
#include "Game/stage/Stage.h"
#include "Engine/3d/Particle3D.h"
#include "Audio/AudioManager.h"
#include "Engine/Tool/GlobalVariables.h"
#include "Application/MapEditor.h"
#include "Application/Particle.h"
#include <array>
#include "Transform.h"
#include "Game/DefaultScene.h"

class GameScene : public DefaultScene
{
public:
	GameScene();
	~GameScene() override;

	void Initialize() override;

	void Update() override;

	void DrawModel() override;

	void DrawSkinningModel() override;

	void DrawParticle() override;

	void DrawSprite() override;

	void DrawLine() override;

private:


private:

	DirectXSetter* dxSetter_ = nullptr;
	Input* input_ = nullptr;
	AudioManager* audioManager_ = nullptr;
	GlobalVariables* gv_ = nullptr;
	MapEditor* mapEditor_ = nullptr;

	std::unique_ptr<Camera> camera_;

	std::unique_ptr<Model> skyDome_;
	std::unique_ptr<Transform> skyDomeTransform_;

	std::unique_ptr<Stage> stage_;

	int32_t resetCount_ = 60;

	uint32_t testSE_;

};


