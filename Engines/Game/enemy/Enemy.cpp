#include "Enemy.h"
#include "Game/stage/Stage.h"
#include "Game/ColorSetter/ColorSetter.h"
#include "ParticleManager.h"
#include "Game/Gimmick/ColorHolder.h"

Enemy::Enemy()
{
	model_.reset(Model::Create("./Resources/enemy/enemy.obj"));
	modelWire_.reset(Model::Create("./Resources/enemy/enemy_wire.obj"));
	collider_ = std::make_unique<BoxCollider>();
	lineBox_ = std::make_unique<LineBox>();
}

Enemy::~Enemy()
{
}

void Enemy::Initialize() {

	name_ = "enemy";
	collider_->SetGameObject(this);
	collider_->collider_.center = transform_->translate_;
	collider_->collider_.size = transform_->scale_;
	collider_->SetFunction([this](Collider* collider) {OnCollision(collider); });
	collider_->SetCollisionAttribute(0x00000002);
	collider_->SetCollisionMask(0xfffffffd);
	lineBox_->SetOBB(&collider_->collider_);

}

void Enemy::Update() {

	model_->material_->constMap_->edgeColor = CreateVector3Color(color_);

	//ステージの色と同じなら起動
	if (GameColor::CheckIsActiveColor(color_, Stage::stageColor_, ColorHolder::GetHolderColor())) {
		
		if (model_->material_->constMap_->threshold > 0.0f) {

			model_->material_->constMap_->threshold -= changeValue_;

			if (model_->material_->constMap_->threshold < 0.0f) {
				model_->material_->constMap_->threshold = 0.0f;
			}

		}
		
		collider_->SetIsActive(true);

		transform_->rotate_.x += changeValue_;
		transform_->rotate_.y += changeValue_;

		if (transform_->rotate_.y > kMaxRotate_) {
			transform_->rotate_.y = 0.0f;
		}

		if (transform_->rotate_.x > kMaxRotate_) {
			transform_->rotate_.x = 0.0f;
		}

		transform_->rotateQuaternion_ = ConvertFromEuler(transform_->rotate_);

	}
	//違うなら判定を消す
	else {

		if (model_->material_->constMap_->threshold < maxValue_) {

			model_->material_->constMap_->threshold += changeValue_;

			if (model_->material_->constMap_->threshold > maxValue_) {
				model_->material_->constMap_->threshold = maxValue_;
			}

		}

		collider_->SetIsActive(false);
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

void Enemy::Draw(Camera* camera) {

	model_->Draw(camera);
	modelWire_->Draw(camera);
	lineBox_->Draw(camera);

}

void Enemy::OnCollision(Collider* collider) {

	if (collider->GetGameObject()->GetName() == "player" || collider->GetGameObject()->GetName() == "copyCat") {

		isDead_ = true;


	}

}
