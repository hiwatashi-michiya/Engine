#include "Switch.h"
#include "Game/stage/Stage.h"
#include "Game/ColorSetter/ColorSetter.h"
#include "Particle.h"
#include "ParticleManager.h"

Switch::Switch()
{
	model_.reset(Model::Create("./Resources/bucket/bucket.obj"));
	modelWire_.reset(Model::Create("./Resources/bucket/bucket_wire.obj"));
	modelTransform_ = std::make_unique<Transform>();
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

	preIsEntered_ = isEntered_;

	//クールタイムカウント
	if (countTimer_ > 0) {
		countTimer_--;
	}

	//座標更新
	collider_->collider_.center = transform_->translate_;
	collider_->collider_.size = transform_->scale_;

	//追加スケールが0.0f以外の場合
	if (addScale_.x >= 0.0f) {
		float changeValue = 0.05f;
		addScale_ -= {changeValue, changeValue, changeValue};

		if (addScale_.x <= 0.0f) {
			addScale_ = Vector3::Zero();
		}

	}

	//モデル用のスケールと平行移動
	modelTransform_->scale_ = transform_->scale_ + addScale_;
	modelTransform_->translate_ = transform_->translate_;

	transform_->UpdateMatrix();
	modelTransform_->UpdateMatrix();

	lineBox_->Update();

	model_->SetWorldMatrix(modelTransform_->worldMatrix_);

	model_->SetColor(CreateColor(color_));

	isEntered_ = false;

}

void Switch::Draw(Camera* camera) {

	model_->Draw(camera);
	lineBox_->Draw(camera);

}

void Switch::OnCollision(Collider* collider) {

	//プレイヤーが触れた場合
	if (collider->GetGameObject()->GetName() == "player" or collider->GetGameObject()->GetName() == "copyCat" or
		collider->GetGameObject()->GetName() == "P_Dive") {

		isEntered_ = true;

		if (isEntered_ != preIsEntered_) {
			//サイズを大きくする
			float scaleValue = 0.5f;
			addScale_ = { scaleValue,scaleValue,scaleValue };
		}

		//クールタイムを終えていたら切り替え発動
		if (countTimer_ <= 0) {
			Stage::stageColor_ = color_;
		}

		countTimer_ = coolTime_;

	}

}
