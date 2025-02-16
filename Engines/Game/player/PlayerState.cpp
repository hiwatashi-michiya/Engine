#include "PlayerState.h"
#include "Player.h"
#include "ImGuiManager.h"
#include "Game/Camera/FollowCamera.h"
#include <vector>
#include "Particle.h"
#include "ParticleManager.h"
#include "Game/ColorSetter/ColorSetter.h"
#include "Game/stage/Stage.h"


void PlayerStay::Initialize(Player* player)
{
	//インプットのインスタンスを渡す
	input_ = Input::GetInstance();

	//ポインタ存在確認
	if (not player) {
		return;
	}

	//プレイヤーを渡す
	player_ = player;

	//モデルのアニメーションリセット、移動をリセット
	player_->SetAnimation(0, true, 2.0f);
	player_->SetVelocity({ 0.0f,player_->GetVelocity().y, 0.0f });

}

void PlayerStay::Update()
{

	//ポインタ存在確認
	if (not player_) {
		return;
	}

	//デバッグ用入力処理
#ifdef _DEBUG

	if ((input_->PushKey(DIK_W) or input_->PushKey(DIK_S) or
		input_->PushKey(DIK_A) or input_->PushKey(DIK_D))) {
		player_->SetPlayerState(std::make_unique<PlayerMove>());
	}
	
	//ボタンをトリガーしていたら潜る状態に移行
	else if (input_->TriggerKey(DIK_Q)) {
		//状態を切り替える
		player_->SetPlayerState(std::make_unique<PlayerDive>());
		return;
	}

#endif // _DEBUG

	//移動入力でMoveに遷移。カメラが上から視点の場合はXY両方反応
	if ((fabsf(input_->GetStickValue(Input::LX)) > 0 or fabsf(input_->GetStickValue(Input::LY)) > 0)) {
		player_->SetPlayerState(std::make_unique<PlayerMove>());
	}

	//移動速度更新、落下速度加算
	player_->SetVelocity({ 0.0f, player_->GetVelocity().y + player_->GetFallSpeed(), 0.0f });

	//ボタンをトリガーしていたら潜る状態に移行
	if (input_->TriggerButton(Input::Button::LB)) {
		//状態を切り替える
		player_->SetPlayerState(std::make_unique<PlayerDive>());
		return;
	}
	

}

void PlayerMove::Initialize(Player* player)
{

	//インプットのインスタンスを渡す
	input_ = Input::GetInstance();

	//ポインタ存在確認
	if (not player) {
		return;
	}

	//プレイヤーを渡す
	player_ = player;

	//モデルのアニメーションリセット、移動をリセット
	player_->SetAnimation(1, true, 2.0f);
	player_->SetVelocity({ 0.0f,player_->GetVelocity().y, 0.0f });

}

void PlayerMove::Update()
{

	//ポインタ存在確認
	if (not player_) {
		return;
	}

	Vector3 moveVector{};

	//デバッグ用の入力処理
#ifdef _DEBUG

	if (input_->PushKey(DIK_W)) {
		moveVector.z = 1.0f;
	}
	else if (input_->PushKey(DIK_S)) {
		moveVector.z = -1.0f;
	}

	if (input_->PushKey(DIK_A)) {
		moveVector.x = -1.0f;
	}
	else if (input_->PushKey(DIK_D)) {
		moveVector.x = 1.0f;
	}

	//ボタンをトリガーしていたら潜る状態に移行
	if (input_->TriggerKey(DIK_Q)) {
		//状態を切り替える
		player_->SetPlayerState(std::make_unique<PlayerDive>());
		return;
	}

#endif // _DEBUG

	//移動している時
	if (fabsf(input_->GetStickValue(Input::LX)) > 0 or 
		fabsf(input_->GetStickValue(Input::LY)) > 0 or
		fabsf(moveVector.x) > 0.0f or fabsf(moveVector.z) > 0.0f) {

		//ボタンをトリガーしていたら潜る状態に移行
		if (input_->TriggerButton(Input::Button::LB)) {
			//状態を切り替える
			player_->SetPlayerState(std::make_unique<PlayerDive>());
			return;
		}

		//X移動量
		if (fabsf(input_->GetStickValue(Input::LX)) > 0) {

			moveVector.x = input_->GetStickValue(Input::LX);

		}
		//Z移動量
		if (fabsf(input_->GetStickValue(Input::LY)) > 0) {

			moveVector.z = input_->GetStickValue(Input::LY);

		}

		//カメラの向きに合わせて移動方向を修正
		if (player_->GetCamera()) {
			moveVector = TransformNormal(Normalize(moveVector), player_->GetCamera()->matRotate_);
		}

		//正規化し、速度を掛ける
		moveVector = Normalize(Vector3{ moveVector.x,0.0f,moveVector.z });
		moveVector *= player_->GetSpeed();

		//回転処理用のVector作成
		Vector3 moveXZ = { moveVector.x, 0.0f, moveVector.z };

		//プレイヤーの向きを更新
		player_->GetTransform()->rotateQuaternion_ =
			RotateForAxis(player_->GetTransform()->rotateQuaternion_, Vector3::AxisZ(), moveXZ, 0.5f);

		//移動速度更新、落下速度加算
		player_->SetVelocity({ moveVector.x, player_->GetVelocity().y + player_->GetFallSpeed(), moveVector.z});

	}
	//移動していない場合は待機状態に遷移
	else {
		player_->SetPlayerState(std::make_unique<PlayerStay>());
	}

}

void PlayerDive::Initialize(Player* player)
{

	//インプットのインスタンスを渡す
	input_ = Input::GetInstance();

	//ポインタ存在確認
	if (not player) {
		return;
	}

	//プレイヤーを渡す
	player_ = player;

	//モデルのアニメーションリセット、移動をリセット
	player_->SetAnimation(3, true, 2.0f);
	player_->SetVelocity({ 0.0f, 0.0f, 0.0f });
	player_->GetCollider()->collider_.size = { 0.9f,0.9f,0.9f };


	//ブロックに潜っている判定もtrue
	player_->SetIsDiving(true);
	//カウントセット
	player_->SetDiveCount(startDiveCount_);
	//プレイヤーの向きからvelocityを設定
	player_->SetVelocity(CalcDiveVelocity(player_->GetTransform()->worldMatrix_.GetZAxis()) * player_->GetSpeed() * 2.0f);
	//プレイヤーの向きを更新
	player_->GetTransform()->rotateQuaternion_ =
		RotateForAxis(player_->GetTransform()->rotateQuaternion_, Vector3::AxisZ(), player_->GetVelocity(), 1.0f);
	//タグを変更
	player_->SetTag("P_Dive");
	//パーティクル追加
	Particle* newParticle = nullptr;

	//未使用のパーティクルのポインタを取得する
	if (ParticleManager::GetInstance()->GetUnUsedParticle(newParticle)) {

		newParticle->Load("./Resources/ParticleData/diveStart.json");
		newParticle->SetMinSpeed(-player_->GetVelocity());
		newParticle->SetMaxSpeed(-player_->GetVelocity() * 0.1f);
		newParticle->SetEndColor(CreateColor(Stage::stageColor_));
		newParticle->SetMaxStartColor(CreateColor(Stage::stageColor_));
		//色の範囲最小を計算
		Vector4 minColor = (Vector4{ 1.0f,1.0f,1.0f,1.0f } - CreateColor(Stage::stageColor_)) * 0.2f + CreateColor(Stage::stageColor_);

		newParticle->SetMinStartColor(minColor);
		newParticle->SetFollowPoint(&player_->GetTransform()->translate_);

	}

}

void PlayerDive::Update()
{

	//ポインタ存在確認
	if (not player_) {
		return;
	}

	//潜行時間が残っていたらカウントを減らす
	if (player_->GetDiveCount() >= 0) {
		player_->SetDiveCount(player_->GetDiveCount() - 1);
		return;
	}

	//潜行状態を終了させる
	player_->SetVelocity({ 0.0f,0.0f,0.0f });
	player_->GetCollider()->collider_.size = { 1.0f,1.0f,1.0f };
	//タグを変更
	player_->SetTag("player");
	//潜っている判定をfalse
	player_->SetIsDiving(false);
	player_->SetPlayerState(std::make_unique<PlayerStay>());

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
	//最も近い向きを返す
	return closestDirection;

}
