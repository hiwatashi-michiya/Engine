#include "Block.h"

Block::Block()
{
	model_.reset(Model::Create("./resources/block/block.obj"));
}

Block::~Block()
{
}

void Block::Initialize(const Color& color) {

	color_ = color;

	if (color_ == kNone) {
		isRock_ = true;
	}
	else {

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

}

void Block::Update() {

	collision_.max = model_->position_ + model_->scale_ / 2.0f;
	collision_.min = model_->position_ - model_->scale_ / 2.0f;

}

void Block::Draw(Camera* camera) {

	model_->Draw(camera);

}
