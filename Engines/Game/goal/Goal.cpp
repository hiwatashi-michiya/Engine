#include "Goal.h"

Goal::Goal()
{
	model_.reset(Model::Create("./resources/goal/goal.obj"));
	tex1_ = TextureManager::GetInstance()->Load("./resources/goal/particle.png");
	tex2_ = TextureManager::GetInstance()->Load("./resources/goal/particle_2.png");
	transform_ = std::make_unique<Transform>();
}

Goal::~Goal()
{
}

void Goal::Initialize(const Color& color) {

	particle_ = std::make_unique<Particle>();
	particle_->Initialize(Particle::k3D, Particle::kUp);
	particle_->startPosition_ = { 0.0f,0.0f,0.0f };
	particle_->AddParticle("./resources/plane/plane.obj", tex1_, 10);
	particle_->AddParticle("./resources/plane/plane.obj", tex2_, 10);
	particle_->endY_ = 10.0f;

	color_ = color;

	if (color_ == kRed) {
		model_->material_->constMap_->color = { 1.0f,0.3f,0.3f,1.0f };
		particle_->SetColor({ 1.0f,0.3f,0.3f,1.0f });
		particle_->SetColor({ 0.7f,0.1f,0.1f,1.0f }, 1);
	}

	if (color_ == kGreen) {
		model_->material_->constMap_->color = { 0.3f,1.0f,0.3f,1.0f };
		particle_->SetColor({ 0.3f,1.0f,0.3f,1.0f });
		particle_->SetColor({ 0.1f,0.7f,0.1f,1.0f }, 1);
	}

	if (color_ == kBlue) {
		model_->material_->constMap_->color = { 0.3f,0.3f,1.0f,1.0f };
		particle_->SetColor({ 0.3f,0.3f,1.0f,1.0f });
		particle_->SetColor({ 0.1f,0.1f,0.7f,1.0f }, 1);
	}

	if (color_ == kYellow) {
		model_->material_->constMap_->color = { 1.0f,1.0f,0.3f,1.0f };
		particle_->SetColor({ 1.0f,1.0f,0.3f,1.0f });
		particle_->SetColor({ 0.7f,0.7f,0.1f,1.0f }, 1);
	}

	particle_->Reset();
	particle_->SetIsStart(true);

}

void Goal::Update() {

	collision_.max = transform_->translate_ + transform_->scale_;
	collision_.min = transform_->translate_ - transform_->scale_;

	particle_->startPosition_ = transform_->translate_;

	if (isGoal_) {
		particle_->Update();
	}

	transform_->UpdateMatrix();

	model_->SetWorldMatrix(transform_->worldMatrix_);

}

void Goal::Draw(Camera* camera) {

	model_->Draw(camera);

}

void Goal::DrawParticle(Camera* camera) {

	if (isGoal_) {
		particle_->Draw(camera);
	}

}
