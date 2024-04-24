#pragma once
#include "Engine/3d/Model.h"
#include <memory>
#include "Engine/input/Input.h"
#include "Engine/math/Quaternion.h"
#include "Engine/math/Collision.h"
#include "Application/Particle.h"
#include "Transform.h"

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

private:

private:

	Input* input_ = nullptr;

	std::unique_ptr<Model> model_;

	std::unique_ptr<Transform> transform_;

	bool canJump_ = true;

	Quaternion rotation_;

	AABB collision_{};

};

