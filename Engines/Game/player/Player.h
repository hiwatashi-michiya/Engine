#pragma once
#include "Engine/3d/Model.h"
#include "Engine/3d/WorldTransform.h"
#include <memory>
#include "Engine/input/Input.h"

class Player
{
public:
	Player();
	~Player();

	void Initialize();

	void Update();

	void Draw();

	void SetParent(WorldTransform* world) { worldTransformBody_.parent_ = world; }

	const OBB& GetOBB() { return this->obb_; }

	void SetPosition(const Vector3& position) { worldTransformBody_.translation_ = position; }

	void SetPositionY(float pos) {
		worldTransformBody_.translation_.y = pos;
		velocity_.y = 0.0f;
	}

	void SetOBB();

	const WorldTransform& GetWorldTransform() { return worldTransformBody_; }

	const Vector3& GetTranslation() { return worldTransformBody_.translation_; }

private:

	Input* input_ = nullptr;

	std::unique_ptr<Model> modelHead_;
	std::unique_ptr<Model> modelBody_;
	std::unique_ptr<Model> modelL_arm_;
	std::unique_ptr<Model> modelR_arm_;

	WorldTransform worldTransformHead_;
	WorldTransform worldTransformBody_;
	WorldTransform worldTransformL_arm_;
	WorldTransform worldTransformR_arm_;

	Vector3 velocity_{};

	bool canJump_ = true;

	OBB obb_;

};


