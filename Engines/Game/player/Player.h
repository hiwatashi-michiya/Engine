#pragma once
#include "Engine/3d/Model.h"
#include <memory>
#include "Engine/input/Input.h"
#include "Engine/math/Quaternion.h"
#include "Engine/math/Collision.h"
#include "Application/Particle.h"
#include "Transform.h"
#include "Skinning/SkinningModel.h"

class Player
{
public:
	Player();
	~Player();

	void Initialize();

	void Update();

	void Draw(Camera* camera);

	void DrawParticle(Camera* camera);

	void SetPosition(const Vector3& position) { transform_->translate_ = position; }

	const AABB& GetCollision() { return collision_; }

	const Vector3& GetPosition() { return transform_->translate_; }

	const Vector3& GetScale() { return transform_->scale_; }

	void SetVelocityY(float speed) { velocity_.y = speed; }

private:

private:

	Input* input_ = nullptr;

	std::unique_ptr<SkinningModel> model_;

	std::unique_ptr<Transform> transform_;

	bool canJump_ = true;

	Quaternion rotation_;

	AABB collision_{};

	Vector3 velocity_{};

	float speed_ = 0.4f;

};

