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

	model_->StartAnimation(true);

}

void Ring::Obtained() {

	isObtained_ = true;
	model_->LoadAnimation("./Resources/item/item_2.gltf");
	model_->ResetAnimation();
	model_->SetIsLoop(false);
	model_->SetAnimationSpeed(5.0f);

}

void Ring::Update() {

	model_->UpdateAnimation();

	//ゲットした演出アニメーションを終えたら破壊
	if (isObtained_ && model_->IsEndAnimation()) {

		transform_->scale_ = Vector3::Zero();

		isVanish_ = true;

	}

	transform_->UpdateMatrix();
	model_->SetWorldMatrix(transform_->worldMatrix_);

}

void Ring::Draw(Camera* camera) {

	model_->Draw(camera);

}
