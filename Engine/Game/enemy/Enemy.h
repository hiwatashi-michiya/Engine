#pragma once
#include "Engine/3d/Model.h"
#include "Engine/3d/WorldTransform.h"
#include <memory>
#include "Game/player/Player.h"

class Enemy
{
public:
	Enemy();
	~Enemy();

	void Initialize();

	void Update();

	void Draw();

	void SetOBB();

	const OBB& GetOBB() { return this->obb_; }

	void SetPositionY(float pos) {
		worldTransformBody_.translation_.y = pos;
		velocity_.y = 0.0f;
	}

	void Collision(Player* player);

private:

	std::unique_ptr<Model> modelBody_;
	std::unique_ptr<Model> modelL_arm_;
	std::unique_ptr<Model> modelR_arm_;

	WorldTransform worldTransformBody_;
	WorldTransform worldTransformL_arm_;
	WorldTransform worldTransformR_arm_;

	Vector3 velocity_{};

	OBB obb_;

};
