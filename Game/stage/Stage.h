#pragma once
#include "Engine/3d/Model.h"
#include "Engine/3d/WorldTransform.h"
#include <memory>
#include "Game/player/Player.h"
#include "Game/enemy/Enemy.h"

class Stage
{
public:
	Stage();
	~Stage();

	void Initialize();

	void Update();

	void Draw();

	void Collision(Player* player, Enemy* enemy);

	void SetOBB();

private:

	std::unique_ptr<Model> modelFloor_;

	std::unique_ptr<Model> modelGoal_;

	WorldTransform worldTransform_;

	WorldTransform worldTransformGoal_;

	OBB obb_;

	OBB obbGoal_;

};

