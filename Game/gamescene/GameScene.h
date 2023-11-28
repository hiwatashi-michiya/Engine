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
#include <list>
#include "Game/lockon/LockOn.h"

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

	void ResetCamera();

	void ResetEnemy();

	void UpdateGlobalVariables();

	std::unique_ptr<Stage> stage_;

	std::unique_ptr<Player> player_;

	std::list<std::shared_ptr<Enemy>> enemies_;

	std::unique_ptr<LockOn> lockOn_;

	std::unique_ptr<Model> modelSkydome_;

	WorldTransform worldTransformSkydome_;

	//前回向いていた向き
	Vector3 preDirection_{};

	//回転行列
	Matrix4x4 directionToDirection_{};

	bool isLockOnStart_ = false;

};


