#include "SpriteFlag.h"
#include "Rand.h"
#include "Collision.h"
#include "Game/stage/Stage.h"
#include "Game/ColorSetter/ColorSetter.h"
#include "Particle.h"
#include "ParticleManager.h"

SpriteFlag::SpriteFlag()
{

	model_ = std::make_unique<Model>();
	model_->Initialize("./Resources/ghostBox/ghostBox.obj");
	collider_ = std::make_unique<BoxCollider>();
	lineBox_ = std::make_unique<LineBox>();
	modelTransform_ = std::make_unique<Transform>();
}

SpriteFlag::~SpriteFlag()
{
	
}

void SpriteFlag::Initialize() {

	name_ = "spriteFlag";
	collider_->SetGameObject(this);
	collider_->collider_.center = transform_->translate_;
	collider_->collider_.size = transform_->scale_;
	preTranslate_ = collider_->collider_.center;
	collider_->SetFunction([this](Collider* collider) {OnCollision(collider); });
	collider_->SetCollisionAttribute(0x00000002);
	collider_->SetCollisionMask(0xfffffffd);
	lineBox_->SetOBB(&collider_->collider_);

}

void SpriteFlag::Update() {

	preTranslate_ = collider_->collider_.center;
	preIsEntered_ = isEntered_;

	//追加スケールが0.0f以外の場合
	if (addScale_.x >= 0.0f) {
		float changeValue = 0.10f;
		addScale_ -= {changeValue, changeValue, changeValue};

		if (addScale_.x <= 0.0f) {
			addScale_ = Vector3::Zero();
		}

	}

	collider_->collider_.center = transform_->translate_;
	collider_->collider_.size = transform_->scale_;
	//モデル用のスケールと平行移動
	modelTransform_->scale_ = transform_->scale_ + addScale_;
	modelTransform_->translate_ = transform_->translate_;

	transform_->UpdateMatrix();
	modelTransform_->UpdateMatrix();

	lineBox_->SetColor({ 1.0f,1.0f,1.0f,1.0f });
	lineBox_->Update();

	model_->SetWorldMatrix(modelTransform_->worldMatrix_);

	model_->SetColor(CreateColor(color_));

	isEntered_ = false;

}

void SpriteFlag::Draw(Camera* camera) {

	model_->Draw(camera);
	if (collider_->GetIsActive()) {
		lineBox_->Draw(camera);
	}

}

void SpriteFlag::OnCollision(Collider* collider) {

	//プレイヤーが中に侵入してきたら
	if (collider->GetGameObject()->GetName() == "player") {

		

	}

}
