#include "Switch.h"

Switch::Switch()
{
	model_.reset(Model::Create("./Resources/switch/switch.obj"));
}

Switch::~Switch()
{
}

void Switch::Initialize() {

	name_ = "switch";
	collider_->SetGameObject(this);
	collider_->collider_.center = transform_->translate_;
	collider_->collider_.size = transform_->scale_;
	collider_->SetCollisionAttribute(0x00000002);
	collider_->SetCollisionMask(0xfffffffd);
	lineBox_->SetOBB(&collider_->collider_);

}

void Switch::Update() {

}

void Switch::Draw(Camera* camera) {

	model_->Draw(camera);

}
