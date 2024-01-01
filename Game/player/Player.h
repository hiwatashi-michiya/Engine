#pragma once
#include "Engine/3d/Model.h"
#include <optional>
#include "Engine/input/Input.h"
#include "Engine/2d/Sprite.h"
#include "Engine/math/Collision.h"
#include "Engine/manager/AudioManager.h"

//振る舞い
enum class Behavior {
	kRoot, //通常状態
	kAttack, //攻撃中
	kDash, //ダッシュ
};

class Player
{
public:
	Player();
	~Player();

	void Initialize();

	void Update();

	void Draw();

	void DrawTexture();

	const Vector3& GetLocalPosition() { return playerModel_->position_; }

	Vector3 GetPosition() {
		return Vector3{ 
			playerModel_->matWorld_.m[3][0],
			playerModel_->matWorld_.m[3][1] ,
			playerModel_->matWorld_.m[3][2] };
	}

	Vector3 GetPrePosition() {
		return prePosition_;
	}

	void SetPosition(const Vector3& position) { playerModel_->position_ = position; }

	bool GetIsBreak() const { return isBreak_; }

	void AddBullet(uint32_t num);

	Model* GetModel() const { return playerModel_.get(); }

	bool GetIsAttack() const { return isAttack_; }

	void SetIsAttack(bool flag) { isAttack_ = flag; }

	void SetCamera(Camera* camera) { camera_ = camera; }

	const Sphere& GetCollision() { return collision_; }

	void Damage(uint32_t val);

	bool GetIsInvincible() const { return workInvincible_.isInvincible; }

private:

	Input* input_ = nullptr;
	AudioManager* audioManager_ = nullptr;

	std::unique_ptr<Model> playerModel_;

	Camera* camera_ = nullptr;

	//前フレームの位置
	Vector3 prePosition_{};

	//ダッシュ用ワーク
	struct WorkDash {
		//ダッシュ用の媒介変数
		uint32_t dashParamater_ = 0;
		//ダッシュのスピード
		float speed_ = 2.0f;
		//ダッシュ時間
		int32_t dashTime_ = 10;
	};

	//攻撃用ワーク
	struct WorkAttack {
		//ブロック所持数
		uint32_t blockCount_;
		//攻撃間隔(フレーム数)
		uint32_t interval_ = 5;
		//攻撃クールタイム
		uint32_t coolTime_;
	};

	//無敵状態ワーク
	struct WorkInvincible {
		//無敵中かどうか
		bool isInvincible;
		//無敵時間
		int32_t invincibleTimer;
	};

	WorkDash workDash_;

	WorkAttack workAttack_;

	WorkInvincible workInvincible_;

	uint32_t kMaxHp_ = 10;

	int32_t hp_;

	//振る舞い
	Behavior behavior_ = Behavior::kRoot;

	//次の振る舞いリクエスト
	std::optional<Behavior> behaviorRequest_ = std::nullopt;

	//前回向いていた向き
	Vector3 preDirection_{};

	//回転行列
	Matrix4x4 directionToDirection_{};

	//移動速度
	Vector3 velocity_{};

	//ブロック破壊フラグ
	bool isBreak_ = false;

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

	//攻撃
	void Attack();

	bool isAttack_ = false;

	//死亡フラグ
	bool isDead_ = false;

	//当たり判定(球)
	Sphere collision_{};

	Texture* hpTex_;

	std::unique_ptr<Sprite> hpSprite_;

	Texture* ui_1_;
	Texture* ui_2_;
	Texture* ui_3_;

	std::unique_ptr<Sprite> ui_A_;
	std::unique_ptr<Sprite> ui_LB_;
	std::unique_ptr<Sprite> ui_RB_;

	uint32_t dashSE_;
	uint32_t deathSE_;
	uint32_t shootSE_;
	uint32_t crashSE_;

};
