#pragma once
#include "Engine/base/DirectXCommon.h"
#include "Engine/manager/TextureManager.h"
#include "Engine/3d/Model.h"
#include "Engine/3d/WorldTransform.h"
#include "Engine/input/Input.h"
#include "Engine/2d/Sprite.h"
#include <memory>
#include "Game/player/Player.h"
#include "Game/enemy/Enemy.h"
#include "Game/stage/Stage.h"
#include "Engine/3d/Particle3D.h"
#include "Engine/manager/AudioManager.h"
#include "Engine/Tool/GlobalVariables.h"
#include "Engine/Tool/MapEditor.h"
#include "Engine/Tool/Particle.h"

class GameScene
{
public:
	GameScene();
	~GameScene();

	void Initialize();

	void Update();

	void Draw();

private:

	DirectXCommon* dxCommon_ = nullptr;
	Input* input_ = nullptr;
	AudioManager* audioManager_ = nullptr;
	GlobalVariables* gv_ = nullptr;
	MapEditor* mapEditor_ = nullptr;

	std::unique_ptr<Camera> camera_;	
	
	Texture* tex_;
	Texture* tex2_;

	std::unique_ptr<Model> model_;
	std::unique_ptr<Model> model2_;

};


