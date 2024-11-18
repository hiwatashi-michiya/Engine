#include "Switch.h"
#include "Game/stage/Stage.h"
#include "Game/ColorSetter/ColorSetter.h"
#include "ColorHolder.h"
#include "Particle.h"
#include "ParticleManager.h"

Switch::Switch()
{
	model_.reset(Model::Create("./Resources/bucket/bucket.obj"));
	modelWire_.reset(Model::Create("./Resources/bucket/bucket_wire.obj"));
	collider_ = std::make_unique<BoxCollider>();
	lineBox_ = std::make_unique<LineBox>();
}

Switch::~Switch()
{
}

void Switch::Initialize() {

	name_ = "switch";
	collider_->SetGameObject(this);
	collider_->collider_.center = transform_->translate_;
	collider_->collider_.size = transform_->scale_;
	collider_->SetFunction([this](Collider* collider) {OnCollision(collider); });
	collider_->SetCollisionAttribute(0x00000002);
	collider_->SetCollisionMask(0xfffffffd);
	lineBox_->SetOBB(&collider_->collider_);

}

void Switch::Update() {

	model_->material_->constMap_->edgeColor = CreateVector3Color(color_);

	//ステージの色と違ったらスイッチを使用できる
	if (color_ != Stage::stageColor_ and countTimer_ <= 0) {
		
		if (model_->material_->constMap_->threshold > 0.0f) {

			model_->material_->constMap_->threshold -= 0.05f;

			if (model_->material_->constMap_->threshold < 0.0f) {
				model_->material_->constMap_->threshold = 0.0f;
			}

		}
		
	}
	//同じだったら効果なし
	else {
		
		if (model_->material_->constMap_->threshold < 1.0f) {

			model_->material_->constMap_->threshold += 0.05f;

			if (model_->material_->constMap_->threshold > 1.0f) {
				model_->material_->constMap_->threshold = 1.0f;
			}

		}

	}

	if (countTimer_ > 0) {
		countTimer_--;
	}

	collider_->collider_.center = transform_->translate_;
	collider_->collider_.size = transform_->scale_;

	transform_->UpdateMatrix();

	lineBox_->Update();

	model_->SetWorldMatrix(transform_->worldMatrix_);
	modelWire_->SetWorldMatrix(transform_->worldMatrix_);

	model_->SetColor(CreateColor(color_));
	modelWire_->SetColor(CreateColor(color_));

}

void Switch::Draw(Camera* camera) {

	model_->Draw(camera);
	modelWire_->Draw(camera);

}

void Switch::DrawLine(Camera* camera) {

	lineBox_->Draw(camera);

}

void Switch::OnCollision(Collider* collider) {

	if (collider->GetGameObject()->GetName() == "player" || collider->GetGameObject()->GetName() == "copyCat") {

		if (countTimer_ <= 0) {
			Stage::stageColor_ = color_;
		}

		countTimer_ = coolTime_;

	}

}
