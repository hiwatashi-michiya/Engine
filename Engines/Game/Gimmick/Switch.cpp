#include "Switch.h"
#include "Game/stage/Stage.h"
#include "UsefulFunc.h"

Switch::Switch()
{
	model_.reset(Model::Create("./Resources/switch/switch.obj"));
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

	//ステージの色と違ったらスイッチを使用できる
	if (colorNumber_ != Stage::stageColor_) {
		collider_->SetIsActive(true);
		
	}
	//同じだったら効果なし
	else {
		collider_->SetIsActive(false);
	}

	collider_->collider_.center = transform_->translate_;
	collider_->collider_.size = transform_->scale_;

	transform_->UpdateMatrix();

	lineBox_->Update();

	model_->SetWorldMatrix(transform_->worldMatrix_);

	model_->SetColor(CreateColor(colorNumber_));

}

void Switch::Draw(Camera* camera) {

	model_->Draw(camera);

}

void Switch::DrawLine(Camera* camera) {

	lineBox_->Draw(camera);

}

void Switch::OnCollision(Collider* collider) {

	if (collider->GetGameObject()->GetName() == "player") {
		Stage::stageColor_ = colorNumber_;
	}

}
