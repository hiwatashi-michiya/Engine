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

	std::unique_ptr<Model> plane_;

	std::unique_ptr<Particle3D> particle_;

	std::vector<WorldTransform> particleTransforms_;

	WorldTransform worldTransformPlane_;

	std::unique_ptr<Model> plane2_;
	std::unique_ptr<Model> plane3_;
	std::unique_ptr<Model> plane4_;
	Model* plane5_;

	std::unique_ptr<Model> planes_[100];

	WorldTransform worldTransformPlane2_;
	WorldTransform worldTransformPlane3_;
	WorldTransform worldTransformPlane4_;
	WorldTransform worldTransformPlane5_;
	uint32_t audio_;
	uint32_t audio2_;

	Texture* tex_;

	std::unique_ptr<Sprite> sprite_;

};


