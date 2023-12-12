#include "Engine/3d/Model.h"
#include <optional>
#include "Engine/input/Input.h"

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

	Vector3 GetPosition() {
		return Vector3{ 
			playerModel_->matWorld_.m[3][0],
			playerModel_->matWorld_.m[3][1] ,
			playerModel_->matWorld_.m[3][2] };
	}

private:

	Input* input_ = nullptr;

	std::unique_ptr<Model> playerModel_;

	//ダッシュ用ワーク
	struct WorkDash {
		//ダッシュ用の媒介変数
		uint32_t dashParamater_ = 0;
		//ダッシュのスピード
		float speed_ = 1.0f;
		//ダッシュ時間
		int32_t dashTime_ = 15;
	};

	WorkDash workDash_;

	//振る舞い
	Behavior behavior_ = Behavior::kRoot;

	//次の振る舞いリクエスト
	std::optional<Behavior> behaviorRequest_ = std::nullopt;

	//前回向いていた向き
	Vector3 preDirection_{};

	//回転行列
	Matrix4x4 directionToDirection_{};

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

};
