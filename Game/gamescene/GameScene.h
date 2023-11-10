#pragma once
#include "Engine/base/DirectXCommon.h"
#include "Engine/base/TextureManager.h"
#include "Engine/3d/Model.h"
#include "Engine/3d/WorldTransform.h"
#include "Engine/input/Input.h"
#include "Engine/2d/Sprite.h"
#include <memory>
#include "Game/player/Player.h"
#include "Game/enemy/Enemy.h"
#include "Game/stage/Stage.h"

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

	//追従対象の残像座標
	Vector3 interTarget_ = {};

	//目標角度
	float destinationAngleY_ = 0.0f;

	//遅延量
	float delay_ = 0.1f;

	Vector3 CalcOffset();

	void Reset();

	void UpdateGlobalVariables();

	std::unique_ptr<Stage> stage_;

	std::unique_ptr<Player> player_;

	std::unique_ptr<Enemy> enemy_;

	std::unique_ptr<Model> modelSkydome_;

	WorldTransform worldTransformSkydome_;

};


