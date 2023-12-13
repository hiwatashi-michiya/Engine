#include "Bullet.h"
#include "Player.h"

Bullet::Bullet()
{
}

Bullet::~Bullet()
{
}

void Bullet::Initialize(Player* player) {

	player_ = player;

	model_.reset(Model::Create("./resources/cube/cube.obj"));
	model_->scale_ *= 0.3f;

	model_->parent_ = player_->GetModel();

}

void Bullet::Update() {

	if (isShot_ && !isDead_) {
		model_->position_ += velocity_;

		liveTime_--;

		if (liveTime_ <= 0) {
			isDead_ = true;
		}

	}
	else {

	}

}

void Bullet::Shot() {

	model_->parent_ = nullptr;
	model_->position_ = player_->GetPosition();
	Vector3 vel{ 0.0f,0.0f,1.0f };
	liveTime_ = maxLiveTime_;
	model_->scale_ = { 0.5f,0.5f,0.5f };
	velocity_ = TransformNormal(vel, player_->GetModel()->matWorld_);
	isShot_ = true;

}

void Bullet::Draw() {

	model_->Draw();

}
