#include "ColorHolder.h"
#include "Game/stage/Stage.h"
#include "Game/ColorSetter/ColorSetter.h"

GameColor::Color ColorHolder::holderColor_ = GameColor::kWhite;

ColorHolder::ColorHolder()
{
	modelOuter_.reset(Model::Create("./Resources/colorHolder/colorHolder.obj"));
	modelInner_.reset(Model::Create("./Resources/colorHolder/colorHolder_inner.obj"));
	collider_ = std::make_unique<BoxCollider>();
	lineBox_ = std::make_unique<LineBox>();
}

ColorHolder::~ColorHolder()
{
}

void ColorHolder::Initialize() {

	name_ = "colorHolder";
	collider_->SetGameObject(this);
	collider_->collider_.center = transform_->translate_;
	collider_->collider_.size = transform_->scale_;
	collider_->SetFunction([this](Collider* collider) {OnCollision(collider); });
	collider_->SetCollisionAttribute(0x00000002);
	collider_->SetCollisionMask(0xfffffffd);
	lineBox_->SetOBB(&collider_->collider_);

}

void ColorHolder::Update() {

	if (countTimer_ > 0) {
		countTimer_--;
	}

	collider_->collider_.center = transform_->translate_;
	collider_->collider_.size = transform_->scale_;

	transform_->UpdateMatrix();

	lineBox_->Update();

	if (holderColor_ != -1) {
		modelInner_->SetColor(CreateColor(holderColor_));
	}
	else {
		modelInner_->SetColor({ 1.0f,1.0f,1.0f,1.0f });
	}

	modelOuter_->SetWorldMatrix(transform_->worldMatrix_);
	modelInner_->SetWorldMatrix(transform_->worldMatrix_);

}

void ColorHolder::Draw(Camera* camera) {

	modelOuter_->Draw(camera);
	modelInner_->Draw(camera);

}

void ColorHolder::DrawLine(Camera* camera) {

	lineBox_->Draw(camera);

}

void ColorHolder::OnCollision(Collider* collider) {

	if (collider->GetGameObject()->GetName() == "player" || collider->GetGameObject()->GetName() == "copyCat") {

		if (countTimer_ <= 0) {
			holderColor_ = Stage::stageColor_;
		}

		countTimer_ = coolTime_;

	}

}
