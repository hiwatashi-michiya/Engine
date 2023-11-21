#pragma once
#include "Engine/3d/Model.h"
#include "Engine/3d/WorldTransform.h"
#include <memory>
#include "Game/player/Player.h"
#include "Game/enemy/Enemy.h"
#include <list>

class Stage
{
public:
	Stage();
	~Stage();

	void Initialize();

	void Update();

	void Draw();

	void Collision(Player* player, std::list<std::shared_ptr<Enemy>> enemies);

	void SetOBB();

private:

	std::unique_ptr<Model> modelFloor_[5];

	std::unique_ptr<Model> modelGoal_;

	WorldTransform worldTransforms_[5];

	WorldTransform worldTransformGoal_;

	Vector3 velocity_[2]{};

	OBB obbs_[5];

	OBB obbGoal_;

};

