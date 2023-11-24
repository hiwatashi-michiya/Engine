#pragma once
#include "Engine/3d/Model.h"
#include "Engine/3d/WorldTransform.h"
#include <memory>
#include "Engine/input/Input.h"
#include <optional>

//振る舞い
enum class Behavior {
	kRoot, //通常状態
	kAttack, //攻撃中
	kDash, //ダッシュ
	kJump, //ジャンプ
};

class LockOn;

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

	const OBB& GetAttackOBB() { return this->attackObb_; }

	void SetPosition(const Vector3& position) { worldTransformBody_.translation_ = position; }

	void SetPositionY(float pos) {
		worldTransformBody_.translation_.y = pos;
		fallVelocity_.y = 0.0f;
	}

	void SetOBB();

	const WorldTransform& GetWorldTransform() { return worldTransformBody_; }

	Vector3 GetWorldPosition() {
		return Vector3(worldTransformBody_.matWorld_.m[3][0],
			worldTransformBody_.matWorld_.m[3][1],
			worldTransformBody_.matWorld_.m[3][2]
		);
	}

	const Vector3& GetTranslation() { return worldTransformBody_.translation_; }

	const Vector3& GetRotation() { return worldTransformBody_.rotation_; }

	bool GetIsDash() {

		if (behavior_ == Behavior::kDash) {
			return true;
		}

		return false;

	}

	Vector3 GetDashStartPosition() { return dashStartPosition_; }

	void SetBehavior(Behavior behavior) { behavior_ = behavior; }

	bool GetIsFall() const { return isFall_; }

	//ロックオンのセッター
	void SetLockOn(LockOn* lockOn) { lockOn_ = lockOn; }

	//目標角度
	float destinationAngleY_;

	//コンボの数
	static const int ComboNum_ = 3;

	//各攻撃時間をまとめたもの
	struct AttackTime {
		//振りかぶり時間
		uint32_t firstTime;
		//溜めまでの時間
		uint32_t secondTime;
		//攻撃振りまでの時間
		uint32_t thirdTime;
		//全ての時間合計
		uint32_t totalTime;
	};

	//攻撃中かどうか
	bool IsAttack() {

		if (behavior_ == Behavior::kAttack) {
			return true;
		}

		return false;

	}

	void SetIsHit(bool flag) { isHit_ = flag; }

	bool GetIsHit() const { return isHit_; }

private:

	Input* input_ = nullptr;

	std::unique_ptr<Model> modelHead_;
	std::unique_ptr<Model> modelBody_;
	std::unique_ptr<Model> modelL_arm_;
	std::unique_ptr<Model> modelR_arm_;
	std::unique_ptr<Model> modelWeapon_;

	WorldTransform worldTransformHead_;
	WorldTransform worldTransformBody_;
	WorldTransform worldTransformL_arm_;
	WorldTransform worldTransformR_arm_;
	WorldTransform worldTransformWeapon_;

	//ロックオン
	LockOn* lockOn_ = nullptr;

	//振る舞い
	Behavior behavior_ = Behavior::kRoot;

	//次の振る舞いリクエスト
	std::optional<Behavior> behaviorRequest_ = std::nullopt;

	//ダッシュ用ワーク
	struct WorkDash {
		//ダッシュ用の媒介変数
		uint32_t dashParamater_ = 0;
		//ダッシュのスピード
		float speed_ = 1.0f;
		//ダッシュ時間
		int32_t dashTime_ = 15;
	};

	//攻撃用ワーク
	struct WorkAttack {
		//攻撃ギミックの媒介変数
		uint32_t attackParameter_ = 0;
		int32_t comboIndex = 0;
		int32_t inComboPhase = 0;
		bool comboNext = false;
	};

	//攻撃用定数
	struct ConstAttack {
		//振りかぶりの時間
		uint32_t anticipationTime;
		//溜めの時間
		uint32_t chargeTime;
		//攻撃振りの時間
		uint32_t swingTime;
		//硬直時間
		uint32_t recoveryTime;
		//振りかぶりの移動速さ
		float anticipationSpeed;
		//溜めの移動速さ
		float chargeSpeed;
		//攻撃振りの移動速さ
		float swingSpeed;
	};

	AttackTime EachAttackTime(ConstAttack constAttack);

	//コンボ定数表
	static const std::array<ConstAttack, ComboNum_> kConstAttacks_;
	
	WorkAttack workAttack_;

	WorkDash workDash_;

	Vector3 fallVelocity_{};

	Vector3 dashStartPosition_{};

	//前回向いていた向き
	Vector3 preDirection_{};

	//回転行列
	Matrix4x4 directionToDirection_{};

	bool canJump_ = true;

	//攻撃が当たったかどうか
	bool isHit_ = false;

	//落ちたかどうか
	bool isFall_ = false;

	//通常行動更新
	void BehaviorRootUpdate();

	//通常行動初期化
	void BehaviorRootInitialize();

	//攻撃行動更新
	void BehaviorAttackUpdate();

	//攻撃行動初期化
	void BehaviorAttackInitialize();

	//ダッシュ更新
	void BehaviorDashUpdate();

	//ダッシュ初期化
	void BehaviorDashInitialize();

	//ジャンプ更新
	void BehaviorJumpUpdate();

	//ジャンプ初期化
	void BehaviorJumpInitialize();

	//グローバル変数更新
	void UpdateGlobalVariables();

	int attackFrame = 0;

	//補間係数
	float lerpT_ = 0.0f;

	OBB obb_;

	OBB attackObb_;

};


