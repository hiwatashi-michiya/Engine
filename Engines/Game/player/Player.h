#pragma once
#include "Application/Particle.h"
#include "Model.h"
#include "Input.h"
#include "Collision.h"
#include "Quaternion.h"
#include "Skinning/SkinningModel.h"
#include "Transform.h"
#include <memory>
#include "Particle3D.h"

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

	std::unique_ptr<Particle3D> particle_;

	std::unique_ptr<Transform> transform_;

	bool canJump_ = true;

	Quaternion rotation_;

	AABB collision_{};

	Vector3 velocity_{};

	float speed_ = 0.2f;

	bool isDead_ = false;

	Texture* tex_ = nullptr;

};

