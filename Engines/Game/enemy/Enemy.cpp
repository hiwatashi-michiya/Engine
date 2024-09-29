#include "Enemy.h"
#include "Game/stage/Stage.h"
#include "UsefulFunc.h"
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

	model_->material_->constMap_->edgeColor = CreateVector3Color(colorNumber_);

	//ステージの色と同じなら起動
	if ((colorNumber_ == Stage::stageColor_ || colorNumber_ == ColorHolder::GetHolderColor())) {
		
		if (model_->material_->constMap_->threshold > 0.0f) {

			model_->material_->constMap_->threshold -= 0.05f;

			if (model_->material_->constMap_->threshold < 0.0f) {
				model_->material_->constMap_->threshold = 0.0f;
			}

		}
		
		collider_->SetIsActive(true);

		transform_->rotate_.x += 0.05f;
		transform_->rotate_.y += 0.05f;

		if (transform_->rotate_.y > 6.28f) {
			transform_->rotate_.y = 0.0f;
		}

		if (transform_->rotate_.x > 6.28f) {
			transform_->rotate_.x = 0.0f;
		}

		transform_->rotateQuaternion_ = ConvertFromEuler(transform_->rotate_);

	}
	//違うなら判定を消す
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

	model_->SetColor(CreateColor(colorNumber_));
	modelWire_->SetColor(CreateColor(colorNumber_));

}

void Enemy::Draw(Camera* camera) {

	model_->Draw(camera);
	modelWire_->Draw(camera);

}

void Enemy::DrawLine(Camera* camera) {

	lineBox_->Draw(camera);

}

void Enemy::OnCollision(Collider* collider) {

	if (collider->GetGameObject()->GetName() == "player" || collider->GetGameObject()->GetName() == "copyCat") {

		isDead_ = true;

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

}
