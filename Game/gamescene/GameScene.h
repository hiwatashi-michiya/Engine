#pragma once
#include "Engine/base/DirectXCommon.h"
#include "Engine/manager/TextureManager.h"
#include "Engine/3d/Model.h"
#include "Engine/3d/WorldTransform.h"
#include "Engine/input/Input.h"
#include "Engine/2d/Sprite.h"
#include <memory>
#include "Game/player/Player.h"
#include "Game/stage/Stage.h"
#include "Game/block/block.h"
#include "Engine/3d/Particle3D.h"
#include "Engine/manager/AudioManager.h"
#include <list>
#include "Game/player/Bullet.h"

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

	//追従対象の残像座標
	Vector3 interTarget_ = {};

	//目標角度
	float destinationAngleY_ = 0.0f;
	float destinationAngleX_ = 0.2f;

	//遅延量
	float delay_ = 10.0f;

	Vector3 CalcOffset();

	std::unique_ptr<Player> player_;

	std::unique_ptr<Stage> stage_;

	std::unique_ptr<Sprite> reticle_;

	std::unique_ptr<Model> reticle3D_;

	Texture* tex_;

	std::list<std::shared_ptr<Block>> blocks_;

	std::list<std::shared_ptr<Bullet>> bullets_;

	Vector3 reticlePos_{};

};


