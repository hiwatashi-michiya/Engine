#include "Ring.h"

Ring::Ring()
{
	model_.reset(Model::Create("./Resources/item/item.gltf"));
}

Ring::~Ring()
{
}

void Ring::Initialize(const Vector3& position) {

	model_->LoadAnimation("./Resources/item/item.gltf");
	transform_ = std::make_unique<Transform>();
	transform_->translate_ = position;

	collision_.max = transform_->translate_ + transform_->scale_;
	collision_.min = transform_->translate_ - transform_->scale_;

	model_->StartAnimation();

}

void Ring::Update() {

	//ゲットした時の演出処理
	if (isGet_ && transform_->scale_.x > 0.0f) {

		model_->SetAnimationSpeed(20.0f);

		transform_->scale_ -= {0.03f, 0.03f, 0.03f};

		if (transform_->scale_.x < 0.0f) {
			transform_->scale_ = Vector3::Zero();
		}

		if (transform_->scale_.x <= 0.0f) {
			isVanish_ = true;
		}

	}

	transform_->UpdateMatrix();
	model_->SetWorldMatrix(transform_->worldMatrix_);

}

void Ring::Draw(Camera* camera) {

	model_->Draw(camera);

}
