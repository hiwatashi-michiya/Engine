#include "GhostBox.h"
#include "Rand.h"
#include "Collision.h"
#include "Game/stage/Stage.h"
#include "Game/ColorSetter/ColorSetter.h"
#include "Game/Bullet/Bullet.h"
#include "Particle.h"
#include "ParticleManager.h"

std::list<GhostBox*> GhostBox::boxList_;

GhostBox::GhostBox()
{
	model_.reset(Model::Create("./Resources/block/block.obj"));
	model_->SetTexture("./Resources/block/ghostBox.png");
	collider_ = std::make_unique<BoxCollider>();
	lineBox_ = std::make_unique<LineBox>();
	modelTransform_ = std::make_unique<Transform>();
	boxList_.push_back(this);
}

GhostBox::~GhostBox()
{
	
	auto it = std::find(boxList_.begin(), boxList_.end(), this);

	if (it != boxList_.end()) {
		boxList_.erase(it);
	}

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
	preIsEntered_ = isEntered_;

	//色が一致していない場合
	if (color_ != secondColor_) {

		if (model_->material_->constMap_->threshold < maxValue_) {

			model_->material_->constMap_->threshold += changeValue_;

			//切り替え演出が終わったら色を既存の色に書き換え
			if (model_->material_->constMap_->threshold >= maxValue_) {
				color_ = secondColor_;
				model_->material_->constMap_->threshold = 0.0f;
			}

		}

	}

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

	//プレイヤーが中に侵入してきたら
	if (collider->GetGameObject()->GetName() == "P_Dive") {

		isEntered_ = true;

		//入られた瞬間、色を変えて一瞬サイズを大きくする
		if (isEntered_ != preIsEntered_) {

			color_ = Stage::stageColor_;
			secondColor_ = color_;
			//サイズを大きくする
			float scaleValue = 1.0f;
			addScale_ = { scaleValue,scaleValue,scaleValue };
			//パーティクルを出す
			//パーティクル追加
			std::unique_ptr<Particle> newParticle;
			newParticle = std::make_unique<Particle>();
			newParticle->Initialize();
			newParticle->Load("./Resources/ParticleData/ghostBox.json");
			newParticle->SetEndColor(CreateColor(color_));
			newParticle->SetMaxStartColor(CreateColor(color_));
			//色の濃さ最大値
			float brightness = 0.2f;
			//色の範囲最小を計算
			Vector4 minColor = (Vector4{ 1.0f,1.0f,1.0f,1.0f } - CreateColor(color_)) * brightness + CreateColor(color_);

			newParticle->SetMinStartColor(minColor);
			newParticle->SetTargetPoint(&transform_->translate_);
			//マネージャーに渡す
			ParticleManager::GetInstance()->AddParticle(newParticle);

		}

	}

}
