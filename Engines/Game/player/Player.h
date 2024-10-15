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
#include "PlayerState.h"
#include <optional>

class Player : public GameObject
{
	friend class PlayerStay;
	friend class PlayerMove;
	friend class PlayerShot;
public:

	enum class Behavior {
		kStay,
		kMove,
		kShot,
	};

	Player();
	~Player();

	void Initialize();

	void Update();

	void Draw(Camera* camera);

	void DrawParticle(Camera* camera);



	void SetColliderPosition(const Vector3& position) { collider_->collider_.center = position; }

	void SetPosition(const Vector3& position) {
		transform_->translate_ = position;
		transform_->UpdateMatrix();
		collider_->collider_.center = transform_->worldMatrix_.GetTranslate() + Vector3{ 0.0f, collider_->collider_.size.y, 0.0f };
	}

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

	Quaternion GetRotate() const { return transform_->rotateQuaternion_; }

	void AddRingCount() { ringGetCount_++; }

private:

	void OnCollision(Collider* collider);

private:

	Input* input_ = nullptr;

	Camera* camera_ = nullptr;

	std::optional<Behavior> behaviorRequest_ = std::nullopt;

	Behavior behavior_ = Behavior::kStay;

	std::unique_ptr<SkinningModel> model_;

	std::unique_ptr<Particle3D> particle_;

	std::unique_ptr<LineBox> lineBox_;

	std::unique_ptr<PlayerStay> playerStay_;
	std::unique_ptr<PlayerMove> playerMove_;
	std::unique_ptr<PlayerShot> playerShot_;

	//ゴール判定
	bool isGoal_ = false;

	//ゴールに必要なアイテム数
	int32_t goalCount_ = 0;

	int32_t ringGetCount_ = 0;

	bool canGoal_ = false;

	bool canJump_ = true;

	Quaternion rotation_;

	std::unique_ptr<BoxCollider> collider_;

	Vector3 preTranslate_{};

	Vector3 velocity_{};
	//移動速度
	float speed_ = 0.25f;
	//落下速度
	float fallSpeed_ = -0.1f;
	//落下速度上限
	float fallSpeedLimit_ = -2.0f;
	//死亡フラグ
	bool isDead_ = false;
	//死亡する高さ
	float deadHeight_ = -20.0f;
	//地面判定
	bool onGround_ = false;
	//テクスチャ
	Texture* tex_ = nullptr;

};

