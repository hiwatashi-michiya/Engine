#include "Stage.h"
#include "Engine/math/Collision.h"

Stage::Stage()
{
}

Stage::~Stage()
{
}

void Stage::Initialize() {

	modelFloor_.reset(Model::Create("ground"));

	worldTransform_.scale_ = { 100.0f,1.0f,100.0f };
	worldTransform_.translation_ = { 0.0f,0.0f,0.0f };

	obb_.center = worldTransform_.translation_;
	obb_.orientations[0] = { 1.0f,0.0f,0.0f };
	obb_.orientations[1] = { 0.0f,1.0f,0.0f };
	obb_.orientations[2] = { 0.0f,0.0f,1.0f };
	obb_.size = worldTransform_.scale_;

	modelGoal_.reset(Model::Create("goal"));
	worldTransformGoal_.translation_ = { 0.0f,1.0f,65.0f };
	obbGoal_.center = worldTransformGoal_.translation_;
	obbGoal_.orientations[0] = { 1.0f,0.0f,0.0f };
	obbGoal_.orientations[1] = { 0.0f,1.0f,0.0f };
	obbGoal_.orientations[2] = { 0.0f,0.0f,1.0f };
	obbGoal_.size = worldTransformGoal_.scale_;

}

void Stage::Update() {

	worldTransform_.UpdateMatrix();

	worldTransformGoal_.UpdateMatrix();

	SetOBB();

}

void Stage::Draw() {

	modelFloor_->Draw(worldTransform_);

	modelGoal_->Draw(worldTransformGoal_);

}

void Stage::Collision(Player* player, Enemy* enemy) {

	if (IsCollision(obb_, enemy->GetOBB())) {
		enemy->SetPositionY(worldTransform_.translation_.y + enemy->GetOBB().size.y);
		enemy->SetOBB();
	}

	if (IsCollision(obb_, player->GetOBB())) {
		player->SetPositionY(worldTransform_.translation_.y + player->GetOBB().size.y);
		player->SetOBB();
	}

}

void Stage::SetOBB() {

	obb_.center = worldTransform_.translation_;
	obb_.size = worldTransform_.scale_;

	//回転行列を生成
	Matrix4x4 rotateMatrix = Multiply(MakeRotateXMatrix(worldTransform_.rotation_.x),
		Multiply(MakeRotateYMatrix(worldTransform_.rotation_.y), MakeRotateZMatrix(worldTransform_.rotation_.z)));

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

	obbGoal_.center = worldTransformGoal_.translation_;
	obbGoal_.size = worldTransformGoal_.scale_;

	//回転行列を生成
	Matrix4x4 rotateGoalMatrix = Multiply(MakeRotateXMatrix(worldTransformGoal_.rotation_.x),
		Multiply(MakeRotateYMatrix(worldTransformGoal_.rotation_.y), MakeRotateZMatrix(worldTransformGoal_.rotation_.z)));

	//回転行列から軸を抽出
	obbGoal_.orientations[0].x = rotateGoalMatrix.m[0][0];
	obbGoal_.orientations[0].y = rotateGoalMatrix.m[0][1];
	obbGoal_.orientations[0].z = rotateGoalMatrix.m[0][2];

	obbGoal_.orientations[1].x = rotateGoalMatrix.m[1][0];
	obbGoal_.orientations[1].y = rotateGoalMatrix.m[1][1];
	obbGoal_.orientations[1].z = rotateGoalMatrix.m[1][2];

	obbGoal_.orientations[2].x = rotateGoalMatrix.m[2][0];
	obbGoal_.orientations[2].y = rotateGoalMatrix.m[2][1];
	obbGoal_.orientations[2].z = rotateGoalMatrix.m[2][2];

	obbGoal_.orientations[0] = Normalize(obbGoal_.orientations[0]);
	obbGoal_.orientations[1] = Normalize(obbGoal_.orientations[1]);
	obbGoal_.orientations[2] = Normalize(obbGoal_.orientations[2]);

}
