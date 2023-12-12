#include "Enemy.h"
#include "Engine/math/Collision.h"

Enemy::Enemy()
{
}

Enemy::~Enemy()
{
}

void Enemy::Initialize() {

	modelBody_.reset(Model::Create("enemy"));
	modelL_arm_.reset(Model::Create("enemy_arm"));
	modelR_arm_.reset(Model::Create("enemy_arm"));

	worldTransformBody_.translation_ = { -5.0f, 0.0f,25.0f };
	worldTransformL_arm_.translation_.x = -1.0f;
	worldTransformR_arm_.translation_.x = 1.0f;
	worldTransformL_arm_.parent_ = &worldTransformBody_;
	worldTransformR_arm_.parent_ = &worldTransformBody_;

	obb_.center = worldTransformBody_.translation_;
	obb_.orientations[0] = { 1.0f,0.0f,0.0f };
	obb_.orientations[1] = { 0.0f,1.0f,0.0f };
	obb_.orientations[2] = { 0.0f,0.0f,1.0f };
	obb_.size = worldTransformBody_.scale_ / 2.0f;

}

void Enemy::Update() {

	if (velocity_.y > -5.0f) {
		velocity_.y -= 0.1f;
	}

	{

		float rad = 3.14f / 60.0f;
		worldTransformBody_.rotation_.y -= rad;

		if (worldTransformBody_.rotation_.y >= 6.28f) {
			worldTransformBody_.rotation_.y = 0.0f;
		}

		Vector3 move = { 0.0f, 0.0f, -0.9f };

		move = TransformNormal(move, worldTransformBody_.matWorld_);
		move /= 3.0f;
		worldTransformBody_.translation_ += move;

	}

	worldTransformBody_.translation_ += velocity_;

	worldTransformL_arm_.rotation_.x += 3.14f / 60.0f;
	worldTransformR_arm_.rotation_.x += 3.14f / 60.0f;

	if (worldTransformL_arm_.rotation_.x >= 6.28f) {
		worldTransformL_arm_.rotation_.x = 0.0f;
	}

	if (worldTransformR_arm_.rotation_.x >= 6.28f) {
		worldTransformR_arm_.rotation_.x = 0.0f;
	}

	if (worldTransformBody_.translation_.y <= -10.0f) {
		worldTransformBody_.translation_ = { 0.0f,0.0f,20.0f };
	}

	worldTransformBody_.UpdateMatrix();
	worldTransformL_arm_.UpdateMatrix();
	worldTransformR_arm_.UpdateMatrix();

	SetOBB();

}

void Enemy::Draw() {

	modelBody_->Draw();
	modelL_arm_->Draw();
	modelR_arm_->Draw();

}

void Enemy::SetOBB() {

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

void Enemy::Collision(Player* player) {

	if (IsCollision(obb_, player->GetOBB())) {
		player->SetPosition({ 0.0f,0.0f,0.0f });
	}

}
