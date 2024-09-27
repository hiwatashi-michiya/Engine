#include "Ring.h"
#include "Audio/AudioManager.h"
#include "Game/stage/Stage.h"
#include "UsefulFunc.h"
#include "ParticleManager.h"
#include "Game/Gimmick/ColorHolder.h"

Ring::Ring()
{
	model_.reset(Model::Create("./Resources/item/item.gltf"));
	modelWire_.reset(Model::Create("./Resources/item/wireRing.gltf"));
	collider_ = std::make_unique<BoxCollider>();
	lineBox_ = std::make_unique<LineBox>();
}

Ring::~Ring()
{
}

void Ring::Initialize(const Vector3& position) {

	name_ = "ring";
	model_->LoadAnimation("./Resources/item/item.gltf");
	modelWire_->LoadAnimation("./Resources/item/item.gltf");
	transform_->translate_ = position;

	collider_->SetGameObject(this);
	collider_->collider_.center = transform_->translate_;
	collider_->collider_.size = transform_->scale_;
	collider_->SetIsActive(true);
	collider_->SetFunction([this](Collider* collider) {OnCollision(collider); });
	lineBox_->SetOBB(&collider_->collider_);

	model_->StartAnimation(true);
	modelWire_->StartAnimation(true);

	getSE_ = AudioManager::GetInstance()->LoadInMF("./Resources/SE/ring_get.mp3");

}

void Ring::Obtained() {

	isObtained_ = true;
	model_->LoadAnimation("./Resources/item/item_2.gltf");
	model_->ResetAnimation();
	model_->SetIsLoop(false);
	model_->SetAnimationSpeed(5.0f);
	modelWire_->LoadAnimation("./Resources/item/item_2.gltf");
	modelWire_->ResetAnimation();
	modelWire_->SetIsLoop(false);
	modelWire_->SetAnimationSpeed(5.0f);
	collider_->SetIsActive(false);
	PlaySE();

	std::shared_ptr<Particle> newParticle;
	newParticle = std::make_shared<Particle>();
	newParticle->Initialize();
	newParticle->SetMinSpawnPoint(transform_->translate_ - transform_->scale_);
	newParticle->SetMaxSpawnPoint(transform_->translate_ + transform_->scale_);
	newParticle->SetMinLifeTime(30);
	newParticle->SetMaxLifeTime(60);
	newParticle->SetParticleLifeTime(90);
	newParticle->SetMinSpeed({ -0.05f,0.02f,-0.05f });
	newParticle->SetMaxSpeed({ 0.05f,0.07f,0.05f });
	newParticle->SetMinScale(0.1f);
	newParticle->SetMaxScale(0.5f);
	newParticle->SetChangeScale(-0.02f);
	newParticle->SetColor(CreateColor(colorNumber_));

	ParticleManager::GetInstance()->AddParticle(newParticle);

}

void Ring::PlaySE() {

	AudioManager::GetInstance()->Play(getSE_, 0.8f);

}

void Ring::Update() {

	model_->UpdateAnimation();
	modelWire_->UpdateAnimation();

	//ゲットした演出アニメーションを終えたら破壊
	if (isObtained_ && model_->IsEndAnimation()) {

		transform_->scale_ = Vector3::Zero();

		isVanish_ = true;

	}

	//色が揃っていて取得されていない時
	if ((colorNumber_ == Stage::stageColor_ || colorNumber_ == ColorHolder::GetHolderColor()) && !isObtained_) {

		if (model_->material_->constMap_->threshold > 0.0f) {

			model_->material_->constMap_->threshold -= 0.05f;

			if (model_->material_->constMap_->threshold < 0.0f) {
				model_->material_->constMap_->threshold = 0.0f;
			}

		}

		collider_->SetIsActive(true);
	}
	//取得された時
	else if (isObtained_) {

		if (model_->material_->constMap_->threshold > 0.0f) {

			model_->material_->constMap_->threshold -= 0.05f;

			if (model_->material_->constMap_->threshold < 0.0f) {
				model_->material_->constMap_->threshold = 0.0f;
			}

		}

		collider_->SetIsActive(false);
	}
	//色が揃っていない時
	else {

		if (model_->material_->constMap_->threshold < 1.0f) {

			model_->material_->constMap_->threshold += 0.1f;

			if (model_->material_->constMap_->threshold > 1.0f) {
				model_->material_->constMap_->threshold = 1.0f;
			}

		}

		collider_->SetIsActive(false);
	}

	collider_->collider_.center = transform_->translate_;
	collider_->collider_.size = transform_->scale_;
	collider_->collider_.orientations[0] = transform_->worldMatrix_.GetXAxis();
	collider_->collider_.orientations[1] = transform_->worldMatrix_.GetYAxis();
	collider_->collider_.orientations[2] = transform_->worldMatrix_.GetZAxis();

	lineBox_->Update();

	transform_->UpdateMatrix();
	model_->SetWorldMatrix(transform_->worldMatrix_);
	modelWire_->SetWorldMatrix(transform_->worldMatrix_);

	model_->SetColor(CreateColor(colorNumber_));
	modelWire_->SetColor(CreateColor(colorNumber_));

}

void Ring::Draw(Camera* camera) {

	model_->Draw(camera);
	modelWire_->Draw(camera);

}

void Ring::DrawLine(Camera* camera) {

#ifdef _DEBUG

	lineBox_->Draw(camera);

#endif // _DEBUG

}

void Ring::OnCollision(Collider* collider) {

	if (collider->GetGameObject()->GetName() == "player" || collider->GetGameObject()->GetName() == "copyCat") {
		Obtained();
	}

}
