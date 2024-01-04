#include "Player.h"
#include <math.h>
#include <cmath>
#include "Engine/manager/ImGuiManager.h"

Player::Player()
{

	model_.reset(Model::Create("./resources/cube/cube.obj"));

}

Player::~Player()
{
}

void Player::Initialize() {

	input_ = Input::GetInstance();

	model_->position_ = { 0.0f,0.0f,0.0f };

}

void Player::Update() {

	// 速さ
	const float speed = 0.7f;

	/*if (velocity_.y > -5.0f) {
		velocity_.y -= 0.1f;
	}*/

	// 移動量。Lスティックの入力を取る
	Vector3 move = { float(input_->GetGamepad().sThumbLX), 0.0f, float(input_->GetGamepad().sThumbLY) };
	// 移動量に速さを反映
	move = Multiply(speed, Normalize(move));

	//Matrix4x4 matRotate = MakeRotateYMatrix(Model::worldTransformCamera_.rotation_.y);

	//// 移動ベクトルをカメラの角度だけ回転させる
	//move = TransformNormal(move, matRotate);

	move *= 0.2f;

	// 移動
	model_->position_ += move;
	//落下処理
	model_->position_ += velocity_;

	// 回転
	if (input_->GetGamepad().sThumbLX != 0 || input_->GetGamepad().sThumbLY != 0) {
		model_->rotation_ = 
	}

}

void Player::Draw(Camera* camera) {

	model_->Draw(camera);

}
