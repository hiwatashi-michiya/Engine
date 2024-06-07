#pragma once
#include "Application/Particle.h"
#include "Engine/3d/Model.h"
#include "Engine/input/Input.h"
#include "Collision.h"
#include "Engine/math/Quaternion.h"
#include "Skinning/SkinningModel.h"
#include "Transform.h"
#include <memory>
#include "GameObject.h"

class Player : public GameObject
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

	Vector3 GetPosition() const { return transform_->worldMatrix_.GetTranslate(); }

	const Vector3& GetScale() { return transform_->scale_; }

	void SetVelocityY(float speed) { velocity_.y = speed; }

	bool GetIsDead() const { return isDead_; }

	void SetIsDead(bool flag) { isDead_ = flag; }

	void DrawSkeleton(Camera* camera);

private:

private:

	Input* input_ = nullptr;

	std::unique_ptr<SkinningModel> model_;

	bool canJump_ = true;

	Quaternion rotation_;

	AABB collision_{};

	Vector3 velocity_{};

	float speed_ = 0.1f;

	bool isDead_ = false;

};

