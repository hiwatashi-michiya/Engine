#include "Goal.h"

Goal::Goal()
{
	model_.reset(Model::Create("./resources/goal/goal.obj"));
}

Goal::~Goal()
{
}

void Goal::Initialize(const Color& color) {

	color_ = color;

	if (color_ == kRed) {
		model_->mesh_->material_->constMap_->color = { 1.0f,0.3f,0.3f,1.0f };
	}

	if (color_ == kGreen) {
		model_->mesh_->material_->constMap_->color = { 0.3f,1.0f,0.3f,1.0f };
	}

	if (color_ == kBlue) {
		model_->mesh_->material_->constMap_->color = { 0.3f,0.3f,1.0f,1.0f };
	}

	if (color_ == kYellow) {
		model_->mesh_->material_->constMap_->color = { 1.0f,1.0f,0.3f,1.0f };
	}

}

void Goal::Update() {

	collision_.max = model_->position_ + model_->scale_;
	collision_.min = model_->position_ - model_->scale_;

}

void Goal::Draw(Camera* camera) {

	model_->Draw(camera);

}
