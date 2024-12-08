#include "Block.h"
#include "Rand.h"
#include "Game/stage/Stage.h"

Block::Block()
{
	model_.reset(Model::Create("./Resources/block/block.obj"));
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
	collider_->collider_.size = transform_->scale_;
	collider_->SetCollisionAttribute(0x00000002);
	collider_->SetCollisionMask(0xfffffffd);
	collider_->SetFunction([this](Collider* collider) {OnCollision(collider); });
	lineBox_->SetOBB(&collider_->collider_);

}

void Block::Update() {

	model_->material_->constMap_->edgeColor = CreateVector3Color(secondColor_);
	model_->material_->constMap_->secondColor = CreateColor(secondColor_);

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

	//色が一致していない場合
	if (color_ != secondColor_) {

		if (model_->material_->constMap_->threshold < 1.0f) {

			model_->material_->constMap_->threshold += 0.02f;

			//切り替え演出が終わったら色を既存の色に書き換え
			if (model_->material_->constMap_->threshold >= 1.0f) {
				color_ = secondColor_;
				model_->material_->constMap_->threshold = 0.0f;
			}

		}

	}

	collider_->collider_.center = transform_->translate_;
	collider_->collider_.size = transform_->scale_;

	model_->material_->pLightMap_->intensity = pLightIntensity_;

	transform_->UpdateMatrix();

	lineBox_->Update();

	model_->SetColor(CreateColor(color_));
	model_->SetWorldMatrix(transform_->worldMatrix_);

}

void Block::Draw(Camera* camera) {

	model_->Draw(camera);
	lineBox_->Draw(camera);

}

void Block::StartColorChange(GameColor::Color color)
{

	//色が同じ場合は処理不要
	if (secondColor_ == color) {
		return;
	}

	//現在の色と第二の色が違う場合は、第二の色を現在の色にしておく
	if (color_ != secondColor_) {
		color_ = secondColor_;
	}

	secondColor_ = color;

	//切り替え演出の初期化
	model_->material_->constMap_->threshold = 0.0f;

}

void Block::OnCollision(Collider* collider)
{

	if (collider->GetGameObject()->GetName() == "player") {

		StartColorChange(Stage::stageColor_);

	}

}

