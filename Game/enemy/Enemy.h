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
		fallVelocity_.y = 0.0f;
	}

	void Collision(Player* player);

	void SetIsDead(bool flag) { isDead_ = flag; }

	bool GetIsDead() { return isDead_; }

	void SetPosition(const Vector3& position) { worldTransformBody_.translation_ = position; }

	const Vector3& GetPosition() { return worldTransformBody_.translation_; }

	Vector3 GetWorldPosition() {
		
		Vector3 pos = {
			worldTransformBody_.matWorld_.m[3][0],
			worldTransformBody_.matWorld_.m[3][1],
			worldTransformBody_.matWorld_.m[3][2]
		};
		
		return pos;

	}

	Vector3 GetModelWorldPos() {

		Vector3 offset = { 0.0f,1.5f,0.0f };

		Vector3 pos = CoordTransform(offset, worldTransformBody_.matWorld_);

		return pos;

	}

	void SetVelocity(const Vector3& velocity) { velocity_ = velocity; }

	void SetMaxMoveTimer(uint32_t time) { maxMoveTime_ = time; }

	void ResetMoveTimer() { moveTimer_ = 0; }

private:

	std::unique_ptr<Model> modelBody_;
	std::unique_ptr<Model> modelL_arm_;
	std::unique_ptr<Model> modelR_arm_;

	WorldTransform worldTransformBody_;
	WorldTransform worldTransformL_arm_;
	WorldTransform worldTransformR_arm_;

	Vector3 fallVelocity_{};

	Vector3 velocity_{};

	uint32_t moveTimer_ = 0;

	uint32_t maxMoveTime_ = 30;

	//硬直中のカウント
	/*uint32_t stunTime_ = 0;*/

	//硬直時間
	uint32_t maxStunTime_ = 30;

	OBB obb_;

	bool isDead_ = false;

	uint32_t hitCount_ = 0;

};
