#include "Block.h"
#include "Rand.h"

Block::Block()
{
	model_.reset(Model::Create("./resources/block/block.obj"));
	transform_ = std::make_unique<Transform>();
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
			model_->material_->constMap_->color = { 1.0f,0.3f,0.3f,1.0f };
		}

		if (color_ == kGreen) {
			model_->material_->constMap_->color = { 0.3f,1.0f,0.3f,1.0f };
		}

		if (color_ == kBlue) {
			model_->material_->constMap_->color = { 0.3f,0.3f,1.0f,1.0f };
		}

		if (color_ == kYellow) {
			model_->material_->constMap_->color = { 1.0f,1.0f,0.3f,1.0f };
		}

	}

	setCount_ = (rand() % 10) * 5 + 10;

}

void Block::Update() {

	//光源の点滅
	if (lightCount_ <= 0) {

		if (pLightIntencity_ > 0.5f) {

			pLightIntencity_ -= 0.01f;

			if (pLightIntencity_ < 0.5f) {
				pLightIntencity_ = 0.5f;
			}

		}

	}
	else if (lightCount_ > 0) {

		if (pLightIntencity_ < 1.0f) {

			pLightIntencity_ += 0.01f;

			if (pLightIntencity_ > 1.0f) {
				pLightIntencity_ = 1.0f;
			}

		}

		lightCount_--;

	}

	//ランダムで明かりを点滅させる
	if (--setCount_ <= 0) {

		int32_t random = rand() % 33;

		if (random == 0) {
			lightCount_ = 30;
		}

		setCount_ = (rand() % 10) * 5 + 10;

	}

	collision_.max = transform_->translate_ + transform_->scale_ / 2.0f;
	collision_.min = transform_->translate_ - transform_->scale_ / 2.0f;

	model_->material_->pLightMap_->intensity = pLightIntencity_;

	//ゴールに入った特定の色ブロックを光らせる
	if (color_ != kNone && isRock_) {
		model_->material_->pLightMap_->intensity = 2.5f;
	}

	transform_->UpdateMatrix();

	model_->SetWorldMatrix(transform_->worldMatrix_);

}

void Block::Draw(Camera* camera) {

	model_->Draw(camera);

}