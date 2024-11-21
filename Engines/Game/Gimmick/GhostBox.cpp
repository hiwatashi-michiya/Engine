#include "GhostBox.h"
#include "Rand.h"
#include "Collision.h"
#include "Game/stage/Stage.h"
#include "Game/ColorSetter/ColorSetter.h"
#include "ColorHolder.h"
#include "Game/Bullet/Bullet.h"

GhostBox::GhostBox()
{
	model_.reset(Model::Create("./Resources/block/block.obj"));
	modelWire_.reset(Model::Create("./Resources/block/block_wire.obj"));
	collider_ = std::make_unique<BoxCollider>();
	lineBox_ = std::make_unique<LineBox>();

}

GhostBox::~GhostBox()
{
}

void GhostBox::Initialize() {

	name_ = "ghostBox";
	collider_->SetGameObject(this);
	collider_->collider_.center = transform_->translate_;
	collider_->collider_.size = transform_->scale_;
	preTranslate_ = collider_->collider_.center;
	collider_->SetFunction([this](Collider* collider) {OnCollision(collider); });
	lineBox_->SetOBB(&collider_->collider_);

}

void GhostBox::Update() {

	model_->material_->constMap_->edgeColor = CreateVector3Color(color_);

	preTranslate_ = collider_->collider_.center;

	if (GameColor::CheckIsActiveColor(color_, Stage::stageColor_, ColorHolder::GetHolderColor())) {

		if (model_->material_->constMap_->threshold > 0.0f) {

			model_->material_->constMap_->threshold -= 0.05f;

			if (model_->material_->constMap_->threshold < 0.0f) {
				model_->material_->constMap_->threshold = 0.0f;
			}

		}

		collider_->SetIsActive(true);
	}
	else {

		if (model_->material_->constMap_->threshold < 1.0f) {

			model_->material_->constMap_->threshold += 0.05f;

			if (model_->material_->constMap_->threshold > 1.0f) {
				model_->material_->constMap_->threshold = 1.0f;
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

void GhostBox::Draw(Camera* camera) {

	model_->Draw(camera);
	modelWire_->Draw(camera);
	if (collider_->GetIsActive()) {
		lineBox_->Draw(camera);
	}

}

void GhostBox::OnCollision(Collider* collider) {

	//プレイヤーの弾に当たったら
	if (collider->GetGameObject()->GetName() == "P_Bullet") {

		//弾の色に変わる
		if (auto pBullet = dynamic_cast<PlayerBullet*>(collider->GetGameObject())) {
			color_ = pBullet->GetBulletColor();
		}

	}

}
