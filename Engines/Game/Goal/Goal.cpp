#include "Goal.h"

Goal::Goal()
{
	model_.reset(Model::Create("./resources/goal/goal.obj"));
	collider_ = std::make_unique<BoxCollider>();
	lineBox_ = std::make_unique<LineBox>();
}

Goal::~Goal()
{
}

void Goal::Initialize() {

	name_ = "goal";
	collider_->SetGameObject(this);
	collider_->collider_.center = transform_->translate_;
	collider_->collider_.size = transform_->scale_;
	collider_->SetCollisionAttribute(0x00000002);
	collider_->SetCollisionMask(0xfffffffd);
	lineBox_->SetOBB(&collider_->collider_);

}

void Goal::Update() {

	collider_->collider_.center = transform_->translate_;
	collider_->collider_.size = transform_->scale_;

	transform_->UpdateMatrix();

	lineBox_->Update();

	model_->SetWorldMatrix(transform_->worldMatrix_);

}

void Goal::Draw(Camera* camera) {
	model_->Draw(camera);
}

void Goal::DrawLine(Camera* camera) {
	lineBox_->Draw(camera);
}
