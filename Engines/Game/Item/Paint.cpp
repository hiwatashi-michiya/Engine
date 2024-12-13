#include "Paint.h"
#include "Audio/AudioManager.h"
#include "Game/stage/Stage.h"
#include "Game/ColorSetter/ColorSetter.h"
#include "ParticleManager.h"
#include "Game/Gimmick/ColorHolder.h"

const std::string Paint::objectName_ = "paint";

Paint::Paint()
{
	model_.reset(Model::Create("./Resources/paint/paint.gltf"));
	modelWire_.reset(Model::Create("./Resources/paint/paint_wire.gltf"));
	collider_ = std::make_unique<BoxCollider>();
	lineBox_ = std::make_unique<LineBox>();
}

Paint::~Paint()
{
}

void Paint::Initialize(const Vector3& position) {

	name_ = "paint";
	model_->LoadAnimation("./Resources/paint/paint.gltf");
	modelWire_->LoadAnimation("./Resources/paint/paint.gltf");
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

void Paint::Obtained() {

	//取得されたフラグを立てる
	isObtained_ = true;
	//アニメーションを切り替え
	model_->LoadAnimation("./Resources/paint/paint_2.gltf");
	model_->ResetAnimation();
	model_->SetIsLoop(false);
	model_->SetAnimationSpeed(5.0f);
	modelWire_->LoadAnimation("./Resources/paint/paint_2.gltf");
	modelWire_->ResetAnimation();
	modelWire_->SetIsLoop(false);
	modelWire_->SetAnimationSpeed(5.0f);
	//コライダーを非アクティブにする
	collider_->SetIsActive(false);
	//SE流す
	PlaySE();

	//取得された時にパーティクル追加
	std::shared_ptr<Particle> newParticle;
	newParticle = std::make_shared<Particle>();
	newParticle->Initialize();
	newParticle->Load("./Resources/ParticleData/paint.json");
	newParticle->SetMinSpawnPoint(transform_->translate_ - transform_->scale_);
	newParticle->SetMaxSpawnPoint(transform_->translate_ + transform_->scale_);
	newParticle->SetEndColor(CreateColor(color_));
	//マネージャーに渡す
	ParticleManager::GetInstance()->AddParticle(newParticle);

}

void Paint::PlaySE() {
	//マネージャーからSE再生
	AudioManager::GetInstance()->Play(getSE_, 0.8f);

}

void Paint::Update() {

	model_->UpdateAnimation();
	model_->material_->constMap_->edgeColor = CreateVector3Color(color_);
	modelWire_->UpdateAnimation();

	//ゲットした演出アニメーションを終えたら破壊
	if (isObtained_ and model_->IsEndAnimation()) {

		transform_->scale_ = Vector3::Zero();

		isVanish_ = true;

	}

	//色が揃っていて取得されていない時
	if (GameColor::CheckIsActiveColor(color_, Stage::stageColor_, ColorHolder::GetHolderColor()) and !isObtained_) {

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

			model_->material_->constMap_->threshold -= changeValue_;

			if (model_->material_->constMap_->threshold < 0.0f) {
				model_->material_->constMap_->threshold = 0.0f;
			}

		}

		collider_->SetIsActive(false);
	}
	//色が揃っていない時
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
	collider_->collider_.orientations[0] = transform_->worldMatrix_.GetXAxis();
	collider_->collider_.orientations[1] = transform_->worldMatrix_.GetYAxis();
	collider_->collider_.orientations[2] = transform_->worldMatrix_.GetZAxis();

	lineBox_->Update();

	transform_->UpdateMatrix();
	model_->SetWorldMatrix(transform_->worldMatrix_);
	modelWire_->SetWorldMatrix(transform_->worldMatrix_);

	model_->SetColor(CreateColor(color_));
	modelWire_->SetColor(CreateColor(color_));

}

void Paint::Draw(Camera* camera) {

#ifdef _DEBUG

	lineBox_->Draw(camera);

#endif // _DEBUG

	model_->Draw(camera);
	modelWire_->Draw(camera);

}

void Paint::OnCollision(Collider* collider) {

	//プレイヤーか分身に触れたら取得された時の処理を実行
	if (collider->GetGameObject()->GetName() == "player" || collider->GetGameObject()->GetName() == "copyCat") {
		Obtained();
	}

}
