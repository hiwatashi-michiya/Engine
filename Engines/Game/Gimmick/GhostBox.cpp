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

	model_->material_->constMap_->edgeColor = CreateVector3Color(secondColor_);
	model_->material_->constMap_->secondColor = CreateColor(secondColor_);

	preTranslate_ = collider_->collider_.center;

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

	transform_->UpdateMatrix();

	lineBox_->SetColor({ 1.0f,1.0f,1.0f,1.0f });
	lineBox_->Update();

	if (rotateType_ == CommonVariables::RotateType::kClockwise) {
		model_->SetTexture("./Resources/block/clockWise.png");
	}
	else {
		model_->SetTexture("./Resources/block/counterClockWise.png");
	}

	model_->SetWorldMatrix(transform_->worldMatrix_);
	modelWire_->SetWorldMatrix(transform_->worldMatrix_);

	model_->SetColor(CreateColor(color_));
	modelWire_->SetColor(CreateColor(color_));

}

void GhostBox::Draw(Camera* camera) {

	model_->Draw(camera);
	if (collider_->GetIsActive()) {
		lineBox_->Draw(camera);
	}

}

void GhostBox::StartColorChange(GameColor::Color color)
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

void GhostBox::OnCollision(Collider* collider) {

	//プレイヤーの弾に当たったら
	if (collider->GetGameObject()->GetName() == "P_Bullet") {

		//弾の色に変わる
		if (auto pBullet = dynamic_cast<PlayerBullet*>(collider->GetGameObject())) {
			color_ = pBullet->GetBulletColor();
		}

	}

	if (collider->GetGameObject()->GetName() == "P_Dive") {

		auto boxCollider = dynamic_cast<BoxCollider*>(collider);

		if (not IsWrapped(collider_->collider_, boxCollider->collider_)) {
			return;
		}

		model_->SetColor(CreateColor(color_) + CreateColor(color_));
		modelWire_->SetColor(CreateColor(color_) + CreateColor(color_));
	}

}
