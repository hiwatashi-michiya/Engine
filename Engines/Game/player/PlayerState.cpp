#include "PlayerState.h"
#include "Player.h"
#include "ImGuiManager.h"
#include "Game/Camera/FollowCamera.h"
#include "Game/Variables/CommonVariables.h"
#include <vector>

using namespace CommonVariables;

PlayerStay::PlayerStay()
{
}

PlayerStay::~PlayerStay()
{
}

void PlayerStay::InitializeStay(Player& player)
{
	//モデルのアニメーションリセット、移動をリセット
	player.model_->SetAnimation(0, true);
	player.model_->SetAnimationSpeed(2.0f);
	player.velocity_.x = 0.0f;
	player.velocity_.z = 0.0f;

}

void PlayerStay::UpdateStay(Player& player)
{

	//デバッグ用入力処理
#ifdef _DEBUG

	if ((player.input_->PushKey(DIK_W) or player.input_->PushKey(DIK_S) or
		player.input_->PushKey(DIK_A) or player.input_->PushKey(DIK_D)) and
		FollowCamera::GetCameraType() == CameraType::kAbove) {
		player.behaviorRequest_ = Player::Behavior::kMove;
	}
	else if ((player.input_->PushKey(DIK_A) or player.input_->PushKey(DIK_D)) and
		FollowCamera::GetCameraType() == CameraType::kSide){
		player.behaviorRequest_ = Player::Behavior::kMove;
	}
	else if (player.input_->TriggerKey(DIK_SPACE)) {
		player.behaviorRequest_ = Player::Behavior::kShot;
	}
	
#endif // _DEBUG

	//移動入力でMoveに遷移。カメラが上から視点の場合はXY両方反応
	if (FollowCamera::GetCameraType() == CameraType::kAbove and
		(fabsf(player.input_->GetStickValue(Input::LX)) > 0 or fabsf(player.input_->GetStickValue(Input::LY)) > 0)) {

		player.behaviorRequest_ = Player::Behavior::kMove;

	}
	//横から視点の場合の移動入力遷移
	else if (FollowCamera::GetCameraType() == CameraType::kSide and
		fabsf(player.input_->GetStickValue(Input::LX)) > 0) {
		player.behaviorRequest_ = Player::Behavior::kMove;
	}
	//RBボタンで弾発射
	else if (player.input_->TriggerButton(Input::Button::RB)) {
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
	//モデルのアニメーションリセット
	player.model_->SetAnimation(1, true);
	player.model_->SetAnimationSpeed(2.0f);
	player.velocity_.x = 0.0f;
	player.velocity_.z = 0.0f;

}

void PlayerMove::UpdateMove(Player& player)
{

	Vector3 moveVector{};

	//デバッグ用の入力処理
#ifdef _DEBUG

	if (FollowCamera::GetCameraType() == CameraType::kAbove) {

		if (player.input_->PushKey(DIK_W)) {
			moveVector.z = 1.0f;
		}
		else if (player.input_->PushKey(DIK_S)) {
			moveVector.z = -1.0f;
		}

	}

	if (player.input_->PushKey(DIK_A)) {
		moveVector.x = -1.0f;
	}
	else if (player.input_->PushKey(DIK_D)) {
		moveVector.x = 1.0f;
	}

	if (player.input_->TriggerKey(DIK_SPACE)) {
		player.behaviorRequest_ = Player::Behavior::kShot;
	}

#endif // _DEBUG

	//移動している時
	if (fabsf(player.input_->GetStickValue(Input::LX)) > 0 or 
		(fabsf(player.input_->GetStickValue(Input::LY)) > 0 and FollowCamera::GetCameraType() == CameraType::kAbove) or
		fabsf(moveVector.x) > 0.0f or fabsf(moveVector.z) > 0.0f) {

		//Aボタンで弾発射
		if (player.input_->TriggerButton(Input::Button::RB)) {
			player.behaviorRequest_ = Player::Behavior::kShot;
		}

		//X移動量
		if (fabsf(player.input_->GetStickValue(Input::LX)) > 0) {

			moveVector.x = player.input_->GetStickValue(Input::LX);

		}
		//Z移動量
		if (fabsf(player.input_->GetStickValue(Input::LY)) > 0 and FollowCamera::GetCameraType() == CameraType::kAbove) {

			moveVector.z = player.input_->GetStickValue(Input::LY);

		}

		//カメラの向きに合わせて移動方向を修正
		if (player.camera_) {
			moveVector = TransformNormal(Normalize(moveVector), player.camera_->matRotate_);
		}

		//正規化し、速度を掛ける
		moveVector = Normalize(Vector3{ moveVector.x,0.0f,moveVector.z });
		moveVector *= player.speed_;

		//回転処理用のVector作成
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

	//アニメーションが終了した時
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

PlayerDive::PlayerDive()
{
}

PlayerDive::~PlayerDive()
{
}

void PlayerDive::InitializeDive(Player& player)
{

	//速度リセット
	player.velocity_ = { 0.0f,0.0f,0.0f };
	//モデルのアニメーションリセット
	player.model_->SetAnimation(1, true);
	player.model_->SetAnimationSpeed(2.0f);
	//潜水フラグオン
	isDiving_ = true;
	//ブロックに潜っている判定もtrue
	player.isDivingBlock_ = true;
	//プレイヤーの向きからvelocityを設定
	player.velocity_ = CalcDiveVelocity(player.transform_->worldMatrix_.GetZAxis()) * player.speed_ * 2.0f;
	player.collider_->collider_.size = { 1.0f,1.0f,1.0f };

}

void PlayerDive::UpdateDive(Player& player)
{

	//潜行しているブロックから抜けていたらフラグを降ろす
	if (not player.isDivingBlock_) {
		isDiving_ = false;
	}

	//潜行していたら
	if ( isDiving_) {
		
	}
	//潜行していなかったら停止状態に移行
	else {
		player.velocity_ = { 0.0f,0.0f,0.0f };
		player.collider_->collider_.size = { 1.0f,1.0f,1.0f };
		player.behaviorRequest_ = Player::Behavior::kStay;
	}



}

Vector3 PlayerDive::CalcDiveVelocity(const Vector3& vec)
{

	//基準
	std::vector<Vector3> directions = {
		{0.0f,0.0f,1.0f}, //前
		{0.0f,0.0f,-1.0f}, //後
		{1.0f,0.0f,0.0f}, //右
		{-1.0f,0.0f,0.0f}, //左
	};

	//正規化した向き
	Vector3 input = Normalize(vec);
	//最も近い方向
	Vector3 closestDirection = directions[0];
	float maxDot = Dot(input, directions[0]);

	for (size_t i = 0; i < directions.size(); i++) {

		float tmpDot = Dot(input, directions[i]);

		if (tmpDot > maxDot) {
			maxDot = tmpDot;
			closestDirection = directions[i];
		}

	}

	return closestDirection;

}
