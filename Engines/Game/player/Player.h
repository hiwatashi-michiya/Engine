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
#include "GameObject.h"
#include "Collider.h"
#include "LineDrawer.h"

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

	BoxCollider* GetCollider() const { return collider_.get(); }

	Vector3 GetPosition() const { return transform_->worldMatrix_.GetTranslate(); }

	const Vector3& GetScale() { return transform_->scale_; }

	void SetVelocityY(float speed) { velocity_.y = speed; }

	bool GetIsDead() const { return isDead_; }

	void SetIsDead(bool flag) { isDead_ = flag; }

	void DrawLine(Camera* camera);

	void SetCamera(Camera* camera) { camera_ = camera; }

private:

	void OnCollision(Collider* collider);

private:

	Input* input_ = nullptr;

	Camera* camera_ = nullptr;

	std::unique_ptr<SkinningModel> model_;

	std::unique_ptr<Particle3D> particle_;

	std::unique_ptr<LineBox> lineBox_;

	bool canJump_ = true;

	Quaternion rotation_;

	std::unique_ptr<BoxCollider> collider_;

	Vector3 velocity_{};

	float speed_ = 0.2f;

	bool isDead_ = false;

	Texture* tex_ = nullptr;

};

