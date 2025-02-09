#include "BombBox.h"
#include "Rand.h"
#include "Collision.h"
#include "Game/stage/Stage.h"
#include "Game/ColorSetter/ColorSetter.h"
#include "Particle.h"
#include "ParticleManager.h"

std::list<BombBox*> BombBox::boxList_;

BombBox::BombBox()
{
	model_ = std::make_unique<Model>();
	model_->Initialize("./Resources/ghostBox/ghostBox.obj");
	collider_ = std::make_unique<BoxCollider>();
	lineBox_ = std::make_unique<LineBox>();
	modelTransform_ = std::make_unique<Transform>();
	boxList_.push_back(this);
}

BombBox::~BombBox()
{

	auto it = std::find(boxList_.begin(), boxList_.end(), this);

	if (it != boxList_.end()) {
		boxList_.erase(it);
	}

}

void BombBox::Initialize() {

	name_ = "bombBox";
	collider_->SetGameObject(this);
	collider_->collider_.center = transform_->translate_;
	collider_->collider_.size = transform_->scale_;
	preTranslate_ = collider_->collider_.center;
	collider_->SetFunction([this](Collider* collider) {OnCollision(collider); });
	collider_->SetCollisionAttribute(0x00000002);
	collider_->SetCollisionMask(0xfffffffd);
	lineBox_->SetOBB(&collider_->collider_);

}

void BombBox::Update() {

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

void BombBox::Draw(Camera* camera) {

	model_->Draw(camera);
	if (collider_->GetIsActive()) {
		lineBox_->Draw(camera);
	}

}

void BombBox::OnCollision(Collider* collider) {

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
