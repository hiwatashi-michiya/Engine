#include "Enemy.h"
#include "Engine/math/Collision.h"
#include "Externals/imgui/imgui.h"

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

#ifdef _DEBUG

	ImGui::Begin("enemy");
	ImGui::DragFloat3("enemyobb", &obb_.center.x, 0.1f);
	ImGui::End();

#endif // _DEBUG


	if (fallVelocity_.y > -5.0f) {
		fallVelocity_.y -= 0.1f;
	}

	{

		/*float rad = 3.14f / 60.0f;
		worldTransformBody_.rotation_.y -= rad;

		if (worldTransformBody_.rotation_.y >= 6.28f) {
			worldTransformBody_.rotation_.y = 0.0f;
		}*/

		if (++moveTimer_ >= maxMoveTime_) {

			worldTransformBody_.rotation_.y += 3.14f;

			if (worldTransformBody_.rotation_.y >= 6.28f) {
				worldTransformBody_.rotation_.y = 0.0f;
			}

			/*velocity_ *= -1.0f;*/
			moveTimer_ = 0;
		}

		Vector3 move = velocity_;

		move = TransformNormal(move, worldTransformBody_.matWorld_);
		move /= 3.0f;
		worldTransformBody_.translation_ += move;

	}

	worldTransformBody_.translation_ += fallVelocity_;

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

	modelBody_->Draw(worldTransformBody_);
	modelL_arm_->Draw(worldTransformL_arm_);
	modelR_arm_->Draw(worldTransformR_arm_);

}

void Enemy::SetOBB() {

	obb_.SetOBB(worldTransformBody_, 1.0f);

}

void Enemy::Collision(Player* player) {

	if (IsCollision(obb_, player->GetOBB())) {

		//ダッシュしていたらダッシュ開始地点に戻す
		if (player->GetIsDash()) {
			player->SetPosition({ 0.0f,0.0f,0.0f });
			player->SetBehavior(Behavior::kRoot);
		}
		//普通に当たったらスタート地点に戻す
		else {
			player->SetPosition({ 0.0f,0.0f,0.0f });
		}
		
	}

	if (IsCollision(obb_, player->GetAttackOBB())) {
		isDead_ = true;
	}

}
