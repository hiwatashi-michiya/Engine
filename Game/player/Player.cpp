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

	isDead_ = false;
	hp_ = kMaxHp_;

	collision_.center = playerModel_->position_;
	collision_.radius = 1.0f;

	directionToDirection_ = MakeIdentity4x4();

	velocity_ = { 0.0f,0.0f,1.0f };

}

void Player::Update() {

	prePosition_ = Vector3{
			playerModel_->matWorld_.m[3][0],
			playerModel_->matWorld_.m[3][1] ,
			playerModel_->matWorld_.m[3][2] };

	isBreak_ = false;

	isAttack_ = false;

	directionToDirection_ = MakeIdentity4x4();

	if (!isDead_) {

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

		if (isBreak_) {

		}
		else {

		}

		if (hp_ <= 0) {
			isDead_ = true;
		}

		collision_.center = playerModel_->position_;

	}

}

void Player::Draw() {

	if (!isDead_) {
		playerModel_->Draw(camera_);
	}

}

void Player::BehaviorRootUpdate() {

	// 速さ
	const float speed = 0.7f;

	//移動入力があった場合
	if (input_->GetGamepad().sThumbLX != 0 || input_->GetGamepad().sThumbLY != 0) {

		// 移動量。Lスティックの入力を取る
		velocity_ = { float(input_->GetGamepad().sThumbLX), 0.0f, float(input_->GetGamepad().sThumbLY) };

		// 移動量に速さを反映
		velocity_ = Multiply(speed, Normalize(velocity_));

		Matrix4x4 matRotate = MakeIdentity4x4();

		matRotate = MakeRotateYMatrix(camera_->rotation_.y);

		/*Matrix4x4 matRotate = MakeRotateYMatrix(Model::worldTransformCamera_.rotation_.y);*/

		// 移動ベクトルをカメラの角度だけ回転させる
		velocity_ = TransformNormal(velocity_, matRotate);

		velocity_ *= 0.8f;

		// 回転
		directionToDirection_ = DirectionToDirection(Normalize(preDirection_), Normalize(velocity_));

		playerModel_->matRotate_ = playerModel_->matRotate_ * directionToDirection_;

		preDirection_ = Normalize(velocity_);

		// 移動
		playerModel_->position_ += velocity_;


	}

	if (input_->TriggerButton(XINPUT_GAMEPAD_A)) {
		isBreak_ = true;
	}

	if (input_->TriggerButton(XINPUT_GAMEPAD_LEFT_SHOULDER)) {
		behaviorRequest_ = Behavior::kDash;
	}

	//攻撃
	if (input_->PushButton(XINPUT_GAMEPAD_RIGHT_SHOULDER)) {
		Attack();
	}

}

void Player::BehaviorRootInitialize() {
	
}

void Player::BehaviorAttackUpdate() {

	behaviorRequest_ = Behavior::kRoot;

}

void Player::BehaviorAttackInitialize() {
	
}

void Player::Attack() {

	if (workAttack_.coolTime_ > 0) {
		workAttack_.coolTime_--;
	}

	if (workAttack_.coolTime_ == 0) {
		
		isAttack_ = true;

	}

}

void Player::AddBullet(uint32_t num) {


}

void Player::BehaviorDashUpdate() {

	Vector3 tmpVel = Normalize(velocity_);

	tmpVel *= workDash_.speed_;

	velocity_ = tmpVel;

	playerModel_->position_ += velocity_;

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
