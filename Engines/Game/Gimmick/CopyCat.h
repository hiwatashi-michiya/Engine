#pragma once
#include "Particle.h"
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
#include "Game/player/Player.h"

class CopyCat : public GameObject
{
public:
	CopyCat();
	~CopyCat();

	void Initialize();

	void Update();

	void Draw(Camera* camera);

	void SetColliderPosition(const Vector3& position) { collider_->collider_.center = position; }

	void SetPosition(const Vector3& position) {
		transform_->translate_ = position;
		transform_->UpdateMatrix();
		collider_->collider_.center = transform_->worldMatrix_.GetTranslate() + Vector3{ 0.0f, collider_->collider_.size.y, 0.0f };
	}

	void SetRespawnPosition(const Vector3& position) { respawnPosition_ = position; }

	BoxCollider* GetCollider() const { return collider_.get(); }

	Vector3 GetPosition() const { return collider_->collider_.center; }

	const Vector3& GetScale() { return transform_->scale_; }

	void SetVelocityY(float speed) { velocity_.y = speed; }

	bool GetIsDead() const { return isDead_; }

	void SetIsDead(bool flag) { isDead_ = flag; }

	void DrawLine(Camera* camera);

	void SetCamera(Camera* camera) { camera_ = camera; }

	bool GetIsGoal() const { return isGoal_; }

	bool GetCanGoal() const { return canGoal_; }

	void SetGoalCount(int32_t count) { goalCount_ = count; }

	Vector3 GetVelocity() const { return velocity_; }

	void SetPlayer(Player* player) { player_ = player; }

	void SetColor(int32_t color) { colorNumber_ = color; }

private:

	void OnCollision(Collider* collider);

private:

	Input* input_ = nullptr;

	Camera* camera_ = nullptr;

	Player* player_ = nullptr;

	std::unique_ptr<SkinningModel> model_;

	std::unique_ptr<LineBox> lineBox_;

	//ゴール判定
	bool isGoal_ = false;

	//ゴールに必要なアイテム数
	int32_t goalCount_ = 0;

	bool canGoal_ = false;

	Quaternion rotation_;

	std::unique_ptr<BoxCollider> collider_;

	Vector3 preTranslate_{};

	Vector3 velocity_{};

	Vector3 respawnPosition_{};

	float speed_ = 0.25f;

	float jumpVelocity_ = 1.5f;

	bool isDead_ = false;

	//地面判定
	bool onGround_ = false;

	int32_t colorNumber_ = 0;

};

