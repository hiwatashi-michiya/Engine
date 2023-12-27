#include "EnemyBullet.h"

EnemyBullet::EnemyBullet()
{
}

EnemyBullet::~EnemyBullet()
{
}

void EnemyBullet::Initialize(const Vector3& position) {

	model_.reset(Model::Create("./resources/cube/cube.obj"));
	model_->scale_ *= 0.7f;

	model_->position_ = position;
	collision_.center = model_->position_;
	collision_.radius = 1.0f;
	model_->rotation_ = { float(rand() % 300) / 100.0f, float(rand() % 300) / 100.0f, float(rand() % 300) / 100.0f };
	model_->matRotate_ = MakeRotateZMatrix(model_->rotation_.z) * MakeRotateYMatrix(model_->rotation_.y) * MakeRotateXMatrix(model_->rotation_.x);

}

void EnemyBullet::Update() {

	if (isShot_ && !isDead_) {
		model_->position_ += velocity_;
		collision_.center = model_->position_;

		liveTime_--;

		if (liveTime_ <= 0) {
			isDead_ = true;
		}

	}
	else {

	}

}

void EnemyBullet::Shot(const Vector3& position) {

	liveTime_ = maxLiveTime_;
	velocity_ = Normalize(position - model_->position_);
	velocity_ *= 2.0f;
	isShot_ = true;

}

void EnemyBullet::Charge(const Vector3& position) {

}

void EnemyBullet::Draw(Camera* camera) {

	model_->Draw(camera);

}
