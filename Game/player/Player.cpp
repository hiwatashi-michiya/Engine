#include "Player.h"
#include <math.h>
#include <cmath>
#include "Engine/manager/ImGuiManager.h"

Player::Player()
{
}

Player::~Player()
{
}

void Player::Initialize() {

	input_ = Input::GetInstance();
	modelBody_.reset(Model::Create("float_body"));
	modelHead_.reset(Model::Create("float_head"));
	modelL_arm_.reset(Model::Create("float_L_arm"));
	modelR_arm_.reset(Model::Create("float_R_arm"));

	worldTransformBody_.translation_.y = 0.0f;
	worldTransformHead_.translation_.y = 3.0f;
	worldTransformL_arm_.translation_ = { 1.0f,2.5f,0.0f };
	worldTransformR_arm_.translation_ = { -1.0f,2.5f,0.0f };

	worldTransformHead_.parent_ = &worldTransformBody_;
	worldTransformL_arm_.parent_ = &worldTransformBody_;
	worldTransformR_arm_.parent_ = &worldTransformBody_;
	worldTransformBody_.isScaleParent_ = false;
	worldTransformBody_.isTranslationParent_ = false;

	obb_.center = worldTransformBody_.translation_;
	obb_.orientations[0] = { 1.0f,0.0f,0.0f };
	obb_.orientations[1] = { 0.0f,1.0f,0.0f };
	obb_.orientations[2] = { 0.0f,0.0f,1.0f };
	obb_.size = worldTransformBody_.scale_ / 2.0f;

}

void Player::Update() {

#ifdef _DEBUG

	ImGui::Begin("player");
	ImGui::DragFloat3("scale", &worldTransformBody_.scale_.x, 0.1f);
	ImGui::DragFloat3("rotation", &worldTransformBody_.rotation_.x, 0.1f);
	ImGui::DragFloat3("translation", &worldTransformBody_.translation_.x, 0.1f);
	ImGui::DragFloat3("scale h", &worldTransformHead_.scale_.x, 0.1f);
	ImGui::DragFloat3("rotation h", &worldTransformHead_.rotation_.x, 0.1f);
	ImGui::DragFloat3("translation h", &worldTransformHead_.translation_.x, 0.1f);
	ImGui::DragFloat3("scale L", &worldTransformL_arm_.scale_.x, 0.1f);
	ImGui::DragFloat3("rotation L", &worldTransformL_arm_.rotation_.x, 0.1f);
	ImGui::DragFloat3("translation L", &worldTransformL_arm_.translation_.x, 0.1f);
	ImGui::DragFloat3("scale R", &worldTransformR_arm_.scale_.x, 0.1f);
	ImGui::DragFloat3("rotation R", &worldTransformR_arm_.rotation_.x, 0.1f);
	ImGui::DragFloat3("translation R", &worldTransformR_arm_.translation_.x, 0.1f);
	ImGui::End();

#endif // _DEBUG

	// 速さ
	const float speed = 0.7f;

	if (velocity_.y > -5.0f) {
		velocity_.y -= 0.1f;
	}

	// 移動量。Lスティックの入力を取る
	Vector3 move = { float(input_->GetGamepad().sThumbLX), 0.0f, float(input_->GetGamepad().sThumbLY) };
	// 移動量に速さを反映
	move = Multiply(speed, Normalize(move));

	Matrix4x4 matRotate = MakeRotateYMatrix(Model::worldTransformCamera_.rotation_.y);

	// 移動ベクトルをカメラの角度だけ回転させる
	move = TransformNormal(move, matRotate);

	move *= 0.2f;

	// 移動
	worldTransformBody_.translation_ += move;
	//落下処理
	worldTransformBody_.translation_ += velocity_;

	// 回転
	if (input_->GetGamepad().sThumbLX != 0 || input_->GetGamepad().sThumbLY != 0) {
		worldTransformBody_.rotation_.y = float(std::atan2(double(move.x), double(move.z)));
	}
	
	if (worldTransformBody_.translation_.y <= -10.0f) {
		worldTransformBody_.translation_ = { 0.0f,0.0f,0.0f };
	}

	if (worldTransformBody_.parent_) {
		
	}

	worldTransformBody_.UpdateMatrix();
	worldTransformHead_.UpdateMatrix();
	worldTransformL_arm_.UpdateMatrix();
	worldTransformR_arm_.UpdateMatrix();

	SetOBB();

}

void Player::Draw() {

	modelBody_->Draw();
	modelHead_->Draw();
	modelL_arm_->Draw();
	modelR_arm_->Draw();

}

void Player::SetOBB() {

	obb_.center = worldTransformBody_.translation_;
	obb_.size = worldTransformBody_.scale_ / 2.0f;

	//回転行列を生成
	Matrix4x4 rotateMatrix = Multiply(MakeRotateXMatrix(worldTransformBody_.rotation_.x),
		Multiply(MakeRotateYMatrix(worldTransformBody_.rotation_.y), MakeRotateZMatrix(worldTransformBody_.rotation_.z)));

	//回転行列から軸を抽出
	obb_.orientations[0].x = rotateMatrix.m[0][0];
	obb_.orientations[0].y = rotateMatrix.m[0][1];
	obb_.orientations[0].z = rotateMatrix.m[0][2];

	obb_.orientations[1].x = rotateMatrix.m[1][0];
	obb_.orientations[1].y = rotateMatrix.m[1][1];
	obb_.orientations[1].z = rotateMatrix.m[1][2];

	obb_.orientations[2].x = rotateMatrix.m[2][0];
	obb_.orientations[2].y = rotateMatrix.m[2][1];
	obb_.orientations[2].z = rotateMatrix.m[2][2];

	obb_.orientations[0] = Normalize(obb_.orientations[0]);
	obb_.orientations[1] = Normalize(obb_.orientations[1]);
	obb_.orientations[2] = Normalize(obb_.orientations[2]);

}