#include "Block.h"
#include "Rand.h"

Block::Block()
{
	model_.reset(Model::Create("./resources/block/block.obj"));
	collider_ = std::make_unique<BoxCollider>();
	lineBox_ = std::make_unique<LineBox>();

}

Block::~Block()
{
}

void Block::Initialize() {

	setCount_ = (rand() % 10) * 5 + 10;

	name_ = "block";
	collider_->SetGameObject(this);
	collider_->collider_.center = transform_->translate_;
	collider_->collider_.size = transform_->scale_ / 2.0f;
	lineBox_->SetOBB(&collider_->collider_);

}

void Block::Update() {

	//光源の点滅
	if (lightCount_ <= 0) {

		if (pLightIntensity_ > 0.5f) {

			pLightIntensity_ -= 0.01f;

			if (pLightIntensity_ < 0.5f) {
				pLightIntensity_ = 0.5f;
			}

		}

	}
	else if (lightCount_ > 0) {

		if (pLightIntensity_ < 1.0f) {

			pLightIntensity_ += 0.01f;

			if (pLightIntensity_ > 1.0f) {
				pLightIntensity_ = 1.0f;
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

	collider_->collider_.center = transform_->translate_;
	collider_->collider_.size = transform_->scale_;

	model_->material_->pLightMap_->intensity = pLightIntensity_;

	transform_->UpdateMatrix();

	lineBox_->Update();

	model_->SetWorldMatrix(transform_->worldMatrix_);

}

void Block::Draw(Camera* camera) {

	model_->Draw(camera);

}

void Block::DrawLine(Camera* camera) {

	lineBox_->Draw(camera);

}
