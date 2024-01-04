#include "Block.h"

Block::Block()
{
}

Block::~Block()
{
}

void Block::Initialize() {



}

void Block::Update() {

	collision_.max = model_->position_ + model_->scale_;
	collision_.min = model_->position_ - model_->scale_;

}

void Block::Draw(Camera* camera) {

	model_->Draw(camera);

}
