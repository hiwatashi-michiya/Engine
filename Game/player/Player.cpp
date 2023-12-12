#include "Player.h"
#include "Engine/input/Input.h"

Player::Player()
{
}

Player::~Player()
{
}

void Player::Initialize() {

	input_ = Input::GetInstance();

	playerModel_.reset(Model::Create("./resources/cube/cube.obj"));
	playerModel_->scale_.z *= 2.0f;

	preDirection_ = { 0.0f,0.0f,1.0f };

	directionToDirection_ = MakeIdentity4x4();

}

void Player::Update() {

	directionToDirection_ = MakeIdentity4x4();

	if (behaviorRequest_) {
		//振る舞いを変更する
		behavior_ = behaviorRequest_.value();
		//各振る舞いごとの初期化を実行
		switch (behavior_) {
		case Behavior::kRoot:
		default:
			BehaviorRootInitialize();
			break;
		case Behavior::kAttack:
			BehaviorAttackInitialize();
			break;
		case Behavior::kDash:
			BehaviorDashInitialize();
			break;
		}
		//振る舞いリクエストをリセット
		behaviorRequest_ = std::nullopt;
	}

	switch (behavior_) {
	case Behavior::kRoot:
	default:
		BehaviorRootUpdate();
		break;
	case Behavior::kAttack:
		BehaviorAttackUpdate();
		break;
	case Behavior::kDash:
		BehaviorDashUpdate();
		break;
	}

}

void Player::Draw() {

	playerModel_->Draw();

}

void Player::BehaviorRootUpdate() {

	// 速さ
	const float speed = 0.7f;

	//移動入力があった場合
	if (input_->GetGamepad().sThumbLX != 0 || input_->GetGamepad().sThumbLY != 0) {

		// 移動量。Lスティックの入力を取る
		Vector3 move = { float(input_->GetGamepad().sThumbLX), 0.0f, float(input_->GetGamepad().sThumbLY) };

		// 移動量に速さを反映
		move = Multiply(speed, Normalize(move));

		Matrix4x4 matRotate = MakeIdentity4x4();

		matRotate = MakeRotateYMatrix(Model::worldTransformCamera_.rotation_.y);

		/*Matrix4x4 matRotate = MakeRotateYMatrix(Model::worldTransformCamera_.rotation_.y);*/

		// 移動ベクトルをカメラの角度だけ回転させる
		move = TransformNormal(move, matRotate);

		move *= 0.4f;

		// 回転
		directionToDirection_ = DirectionToDirection(Normalize(preDirection_), Normalize(move));

		playerModel_->matRotate_ = playerModel_->matRotate_ * directionToDirection_;

		preDirection_ = Normalize(move);

		// 移動
		playerModel_->position_ += move;


	}

	if (input_->TriggerButton(XINPUT_GAMEPAD_RIGHT_SHOULDER)) {
		behaviorRequest_ = Behavior::kAttack;
	}

	if (input_->TriggerButton(XINPUT_GAMEPAD_LEFT_SHOULDER)) {
		behaviorRequest_ = Behavior::kDash;
	}

}

void Player::BehaviorRootInitialize() {
	
}

void Player::BehaviorAttackUpdate() {

	behaviorRequest_ = Behavior::kRoot;

}

void Player::BehaviorAttackInitialize() {
	
}

void Player::BehaviorDashUpdate() {

	//自キャラの向いている方向に移動
	Vector3 move = { sinf(playerModel_->rotation_.y), 0.0f,cosf(playerModel_->rotation_.y) };

	move *= workDash_.speed_;

	playerModel_->position_ += move;

	//ダッシュ時間
	const uint32_t behaviorDashTime = workDash_.dashTime_;

	//既定の時間で通常行動に戻る
	if (++workDash_.dashParamater_ >= behaviorDashTime) {
		behaviorRequest_ = Behavior::kRoot;
	}

}

void Player::BehaviorDashInitialize() {

	workDash_.dashParamater_ = 0;

}
