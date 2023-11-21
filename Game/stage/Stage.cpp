#include "Stage.h"
#include "Engine/math/Collision.h"

Stage::Stage()
{
}

Stage::~Stage()
{
}

void Stage::Initialize() {

	for (int i = 0; i < 5; i++) {
		modelFloor_[i].reset(Model::Create("ground"));

		if (i < 2) {
			worldTransforms_[i].scale_ = { 5.0f,0.1f,5.0f };
			worldTransforms_[i].translation_ = { 0.0f,0.0f, i * 10.0f };
		}
		else if (i == 2) {
			worldTransforms_[i].scale_ = { 100.0f, 0.1f,10.0f };
			worldTransforms_[i].translation_ = { 0.0f,0.0f, i * 20.0f - 15.0f };
		}
		else {
			worldTransforms_[i].scale_ = { 10.0f, 0.1f,10.0f };
			worldTransforms_[i].translation_ = { 0.0f,0.0f, i * 20.0f - 15.0f };
		}

		obbs_[i].center = worldTransforms_[i].translation_;
		obbs_[i].orientations[0] = { 1.0f,0.0f,0.0f };
		obbs_[i].orientations[1] = { 0.0f,1.0f,0.0f };
		obbs_[i].orientations[2] = { 0.0f,0.0f,1.0f };
		obbs_[i].size = worldTransforms_[i].scale_;
	}

	modelGoal_.reset(Model::Create("goal"));
	worldTransformGoal_.translation_ = { 0.0f,1.0f,65.0f };
	obbGoal_.center = worldTransformGoal_.translation_;
	obbGoal_.orientations[0] = { 1.0f,0.0f,0.0f };
	obbGoal_.orientations[1] = { 0.0f,1.0f,0.0f };
	obbGoal_.orientations[2] = { 0.0f,0.0f,1.0f };
	obbGoal_.size = worldTransformGoal_.scale_;

	fallVelocity_[0] = { -0.1f,0.0f,0.0f };
	fallVelocity_[1] = { 0.1f,0.0f,0.0f };

}

void Stage::Update() {

	worldTransforms_[1].translation_ += fallVelocity_[0];
	worldTransforms_[3].translation_ += fallVelocity_[1];

	if (worldTransforms_[1].translation_.x < -10.0f) {
		fallVelocity_[0] *= -1.0f;
	}

	if (worldTransforms_[1].translation_.x > 10.0f) {
		fallVelocity_[0] *= -1.0f;
	}

	if (worldTransforms_[3].translation_.x < -10.0f) {
		fallVelocity_[1] *= -1.0f;
	}

	if (worldTransforms_[3].translation_.x > 10.0f) {
		fallVelocity_[1] *= -1.0f;
	}

	for (int i = 0; i < 5; i++) {
		worldTransforms_[i].UpdateMatrix();
	}

	worldTransformGoal_.UpdateMatrix();

	SetOBB();

}

void Stage::Draw() {

	for (int i = 0; i < 5; i++) {
		modelFloor_[i]->Draw(worldTransforms_[i]);
	}

	modelGoal_->Draw(worldTransformGoal_);

}

void Stage::Collision(Player* player, std::list<std::shared_ptr<Enemy>> enemies) {

	for (size_t i = 0; i < 5; i++) {

		for (auto& enemy : enemies) {

			if (IsCollision(obbs_[i], enemy->GetOBB())) {
				enemy->SetPositionY(worldTransforms_[i].translation_.y + enemy->GetOBB().size.y);
				enemy->SetOBB();
			}

		}

	}

	for (size_t i = 0; i < 5; i++) {

		if (IsCollision(obbs_[i], player->GetOBB())) {
			player->SetParent(&worldTransforms_[i]);
			if (i == 1) {
				player->SetPosition(player->GetTranslation() + fallVelocity_[0]);
			}
			if (i == 3) {
				player->SetPosition(player->GetTranslation() + fallVelocity_[1]);
			}
			player->SetPositionY(worldTransforms_[i].translation_.y + player->GetOBB().size.y);
			player->SetOBB();
			break;
		}
		else {
			player->SetParent(nullptr);
		}

	}

	if (IsCollision(obbGoal_, player->GetOBB())) {
		player->SetPosition({ 0.0f,0.0f,0.0f });
		player->SetOBB();
	}

}

void Stage::SetOBB() {

	for (size_t i = 0; i < 5; i++) {

		obbs_[i].center = worldTransforms_[i].translation_;
		obbs_[i].size = worldTransforms_[i].scale_;

		//回転行列を生成
		Matrix4x4 rotateMatrix = Multiply(MakeRotateXMatrix(worldTransforms_[i].rotation_.x),
			Multiply(MakeRotateYMatrix(worldTransforms_[i].rotation_.y), MakeRotateZMatrix(worldTransforms_[i].rotation_.z)));

		//回転行列から軸を抽出
		obbs_[i].orientations[0].x = rotateMatrix.m[0][0];
		obbs_[i].orientations[0].y = rotateMatrix.m[0][1];
		obbs_[i].orientations[0].z = rotateMatrix.m[0][2];

		obbs_[i].orientations[1].x = rotateMatrix.m[1][0];
		obbs_[i].orientations[1].y = rotateMatrix.m[1][1];
		obbs_[i].orientations[1].z = rotateMatrix.m[1][2];

		obbs_[i].orientations[2].x = rotateMatrix.m[2][0];
		obbs_[i].orientations[2].y = rotateMatrix.m[2][1];
		obbs_[i].orientations[2].z = rotateMatrix.m[2][2];

		obbs_[i].orientations[0] = Normalize(obbs_[i].orientations[0]);
		obbs_[i].orientations[1] = Normalize(obbs_[i].orientations[1]);
		obbs_[i].orientations[2] = Normalize(obbs_[i].orientations[2]);

	}

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
