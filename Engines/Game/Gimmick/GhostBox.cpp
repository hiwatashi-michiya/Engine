#include "GhostBox.h"
#include "Rand.h"
#include "Collision.h"

Segment* GhostBox::hideLine_ = nullptr;

GhostBox::GhostBox()
{
	model_.reset(Model::Create("./Resources/block/block.obj"));
	collider_ = std::make_unique<BoxCollider>();
	lineBox_ = std::make_unique<LineBox>();

}

GhostBox::~GhostBox()
{
}

void GhostBox::Initialize() {

	name_ = "ghostBox";
	collider_->SetGameObject(this);
	collider_->collider_.center = transform_->translate_;
	collider_->collider_.size = transform_->scale_;
	preTranslate_ = collider_->collider_.center;
	collider_->SetFunction([this](Collider* collider) {OnCollision(collider); });
	lineBox_->SetOBB(&collider_->collider_);

}

void GhostBox::Update() {

	if (IsCollision(collider_->collider_, *hideLine_)) {
		collider_->SetIsActive(false);
	}
	else {
		collider_->SetIsActive(true);
	}

	preTranslate_ = collider_->collider_.center;

	collider_->collider_.center = transform_->translate_;
	collider_->collider_.size = transform_->scale_;

	transform_->UpdateMatrix();

	lineBox_->Update();

	model_->SetWorldMatrix(transform_->worldMatrix_);

}

void GhostBox::Draw(Camera* camera) {

	if (collider_->GetIsActive()) {
		model_->Draw(camera);
	}

}

void GhostBox::DrawLine(Camera* camera) {

	if (collider_->GetIsActive()) {
		lineBox_->Draw(camera);
	}

}

void GhostBox::OnCollision(Collider* collider) {

	

}
