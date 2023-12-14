#include "Bullet.h"
#include "Player.h"
#include "Engine/math/Rand.h"

Bullet::Bullet()
{
}

Bullet::~Bullet()
{
}

void Bullet::Initialize(const Vector3& position) {

	model_.reset(Model::Create("./resources/cube/cube.obj"));
	model_->scale_ *= 0.7f;

	model_->position_ = position;
	model_->rotation_ = { float(rand() % 300) / 100.0f, float(rand() % 300) / 100.0f, float(rand() % 300) / 100.0f };
	model_->matRotate_ = MakeRotateZMatrix(model_->rotation_.z) * MakeRotateYMatrix(model_->rotation_.y) * MakeRotateXMatrix(model_->rotation_.x);

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

void Bullet::Shot(const Vector3& position) {

	liveTime_ = maxLiveTime_;
	velocity_ = Normalize(position - model_->position_);
	velocity_ *= 2.0f;
	isShot_ = true;

}

void Bullet::Charge(const Vector3& position) {

}

void Bullet::Draw() {

	model_->Draw();

}
