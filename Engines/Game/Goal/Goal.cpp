#include "Goal.h"
#include "Game/stage/Stage.h"
#include "UsefulFunc.h"

Goal::Goal()
{
	model_.reset(Model::Create("./Resources/goal/goal.obj"));
	particle_.reset(Particle3D::Create("./Resources/particle/particle.obj", 128));
	particle_->SetInstanceCount(32);
	collider_ = std::make_unique<BoxCollider>();
	lineBox_ = std::make_unique<LineBox>();
}

Goal::~Goal()
{
}

void Goal::Initialize() {

	name_ = "goal";
	collider_->SetGameObject(this);
	collider_->collider_.center = transform_->translate_;
	collider_->collider_.size = transform_->scale_;
	collider_->SetCollisionAttribute(0x00000002);
	collider_->SetCollisionMask(0xfffffffd);
	lineBox_->SetOBB(&collider_->collider_);
	isActiveEffect_ = false;

	for (int32_t i = 0; i < particleCount_; i++) {

		particle_->velocities_[i] = { 0.0f,1.0f,0.0f };
		particle_->transforms_[i]->scale_ = { 0.0f,0.0f,0.0f };

	}

}

void Goal::Update() {

	if (!isActiveEffect_ and player_ and player_->GetCanGoal()) {
		isActiveEffect_ = true;
	}

	if (isActiveEffect_) {

		//パーティクル更新
		for (int32_t i = 0; i < 32; i++) {

			if (particle_->transforms_[i]->scale_.y <= 0.0f) {

				particle_->colors_[i] = CreateColor(Stage::stageColor_);
				particle_->velocities_[i] = { float((rand() % 40 - 20) * 0.003f),float((rand() % 40 + 20) * 0.003f), float((rand() % 40 - 20) * 0.003f) };
				particle_->transforms_[i]->translate_ = transform_->translate_ + Vector3{ float((rand() % 3 - 1) * 0.5f),0.0f, float((rand() % 3 - 1) * 0.5f) };
				particle_->transforms_[i]->rotateQuaternion_ = IdentityQuaternion();
				particle_->transforms_[i]->scale_ = { 0.5f,0.5f,0.5f };
				particle_->isActive_[i] = true;
				break;
			}



		}

		for (int32_t i = 0; i < 32; i++) {

			if (particle_->transforms_[i]->scale_.y > 0.0f) {
				particle_->colors_[i] = CreateColor(Stage::stageColor_);
				particle_->transforms_[i]->translate_ += particle_->velocities_[i];
				particle_->transforms_[i]->rotateQuaternion_ = particle_->transforms_[i]->rotateQuaternion_ * ConvertFromEuler(particle_->velocities_[i]);
				particle_->transforms_[i]->scale_ -= {0.02f, 0.02f, 0.02f};
			}
			else {
				particle_->colors_[i].w = 0.0f;
			}

		}

	}

	collider_->collider_.center = transform_->translate_;
	collider_->collider_.size = transform_->scale_;

	transform_->UpdateMatrix();

	lineBox_->Update();

	model_->SetWorldMatrix(transform_->worldMatrix_);
	model_->SetColor(CreateColor(Stage::stageColor_));

}

void Goal::Draw(Camera* camera) {
	model_->Draw(camera);
}

void Goal::DrawParticle(Camera* camera) {

	if (isActiveEffect_) {
		particle_->Draw(camera);
	}

}

void Goal::DrawLine(Camera* camera) {
#ifdef _DEBUG

	lineBox_->Draw(camera);

#endif // _DEBUG
}
