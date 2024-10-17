#include "PlayerState.h"
#include "Player.h"
#include "ImGuiManager.h"

PlayerStay::PlayerStay()
{
}

PlayerStay::~PlayerStay()
{
}

void PlayerStay::InitializeStay(Player& player)
{

	player.model_->SetAnimation(0, true);
	player.model_->SetAnimationSpeed(2.0f);
	player.velocity_.x = 0.0f;
	player.velocity_.z = 0.0f;

}

void PlayerStay::UpdateStay(Player& player)
{

#ifdef _DEBUG

	if (player.input_->PushKey(DIK_W) or player.input_->PushKey(DIK_A) or
		player.input_->PushKey(DIK_S) or player.input_->PushKey(DIK_D)) {
		player.behaviorRequest_ = Player::Behavior::kMove;
	}
	else if (player.input_->TriggerKey(DIK_SPACE)) {
		player.behaviorRequest_ = Player::Behavior::kShot;
	}
	
#endif // _DEBUG

	//移動入力でMoveに遷移
	if (fabsf(player.input_->GetStickValue(Input::LX)) > 0 or fabsf(player.input_->GetStickValue(Input::LY)) > 0) {

		player.behaviorRequest_ = Player::Behavior::kMove;

	}
	//Aボタンで弾発射
	else if (player.input_->TriggerButton(Input::Button::A)) {
		player.behaviorRequest_ = Player::Behavior::kShot;
	}
	

}

PlayerMove::PlayerMove()
{
}

PlayerMove::~PlayerMove()
{
}

void PlayerMove::InitializeMove(Player& player)
{

	player.model_->SetAnimation(1, true);
	player.model_->SetAnimationSpeed(2.0f);

}

void PlayerMove::UpdateMove(Player& player)
{

	Vector3 moveVector{};

#ifdef _DEBUG

	if (player.input_->PushKey(DIK_W)) {
		moveVector.z = 1.0f;
	}
	else if (player.input_->PushKey(DIK_S)) {
		moveVector.z = -1.0f;
	}

	if (player.input_->PushKey(DIK_A)) {
		moveVector.x = -1.0f;
	}
	else if (player.input_->PushKey(DIK_D)) {
		moveVector.x = 1.0f;
	}

#endif // _DEBUG

	if (fabsf(player.input_->GetStickValue(Input::LX)) > 0 or fabsf(player.input_->GetStickValue(Input::LY)) > 0 or
		fabsf(moveVector.x) > 0.0f or fabsf(moveVector.z) > 0.0f) {

		//Aボタンで弾発射
		if (player.input_->TriggerButton(Input::Button::A)) {
			player.behaviorRequest_ = Player::Behavior::kShot;
		}

		if (fabsf(player.input_->GetStickValue(Input::LX)) > 0) {

			moveVector.x = player.input_->GetStickValue(Input::LX);

		}

		if (fabsf(player.input_->GetStickValue(Input::LY)) > 0) {

			moveVector.z = player.input_->GetStickValue(Input::LY);

		}

		if (player.camera_) {
			moveVector = TransformNormal(Normalize(moveVector), player.camera_->matRotate_);
		}

		moveVector = Normalize(Vector3{ moveVector.x,0.0f,moveVector.z });

		moveVector *= player.speed_;

		Vector3 moveXZ = { moveVector.x, 0.0f, moveVector.z };

		//回転処理
		if (fabsf(Length(moveXZ)) > 0.00001f) {

			//一旦正規化
			moveXZ = Normalize(moveXZ);

			Vector3 tmp = ConjuGate(player.transform_->rotateQuaternion_) * moveXZ;

			Quaternion diff = MakeRotateAxisAngleQuaternion(Normalize(Cross({ 0.0f,0.0f,1.0f }, Normalize(tmp))), std::acos(Dot({ 0.0f,0.0f,1.0f }, Normalize(tmp))));

			player.transform_->rotateQuaternion_ = Slerp(IdentityQuaternion(), diff, 0.5f) * player.transform_->rotateQuaternion_;

		}

		//移動速度更新
		player.velocity_.x = moveVector.x;
		player.velocity_.z = moveVector.z;

	}
	//移動していない場合は待機状態に遷移
	else {
		player.behaviorRequest_ = Player::Behavior::kStay;
	}

}

PlayerShot::PlayerShot()
{
}

PlayerShot::~PlayerShot()
{
}

void PlayerShot::InitializeShot(Player& player)
{

	//アニメーション、移動速度リセット
	player.model_->SetAnimation(2, true);
	player.model_->SetAnimationSpeed(4.0f);
	player.velocity_.x = 0.0f;
	player.velocity_.z = 0.0f;

	//プレイヤーの向きに弾の移動方向を設定
	Vector3 bulletVelocity = Normalize(player.transform_->worldMatrix_.GetZAxis());

	//弾を生成し、発射
	std::shared_ptr<PlayerBullet> newBullet;
	newBullet = std::make_shared<PlayerBullet>();
	newBullet->Initialize(bulletVelocity, player.GetPosition(), 60);
	player.bullets_.push_back(newBullet);

}

void PlayerShot::UpdateShot(Player& player)
{

	if (player.model_->IsEndAnimation()) {

		//移動の入力があるならMoveに遷移
		if (fabsf(player.input_->GetStickValue(Input::LX)) > 0 or fabsf(player.input_->GetStickValue(Input::LY)) > 0) {

			player.behaviorRequest_ = Player::Behavior::kMove;

		}
		//ない場合はStayに遷移
		else {
			player.behaviorRequest_ = Player::Behavior::kStay;
		}

	}



}
