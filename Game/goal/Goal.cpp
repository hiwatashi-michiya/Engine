#include "Goal.h"

Goal::Goal()
{
	model_.reset(Model::Create("./resources/goal/goal.obj"));
	tex1_ = TextureManager::GetInstance()->Load("./resources/goal/particle.png");
	tex2_ = TextureManager::GetInstance()->Load("./resources/goal/particle_2.png");
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
		model_->mesh_->material_->constMap_->color = { 1.0f,0.3f,0.3f,1.0f };
		particle_->SetColor({ 1.0f,0.3f,0.3f,1.0f });
	}

	if (color_ == kGreen) {
		model_->mesh_->material_->constMap_->color = { 0.3f,1.0f,0.3f,1.0f };
		particle_->SetColor({ 0.3f,1.0f,0.3f,1.0f });
	}

	if (color_ == kBlue) {
		model_->mesh_->material_->constMap_->color = { 0.3f,0.3f,1.0f,1.0f };
		particle_->SetColor({ 0.3f,0.3f,1.0f,1.0f });
	}

	if (color_ == kYellow) {
		model_->mesh_->material_->constMap_->color = { 1.0f,1.0f,0.3f,1.0f };
		particle_->SetColor({ 1.0f,1.0f,0.3f,1.0f });
	}

	particle_->Reset();
	particle_->SetIsStart(true);

}

void Goal::Update() {

	collision_.max = model_->position_ + model_->scale_;
	collision_.min = model_->position_ - model_->scale_;

	particle_->startPosition_ = model_->position_;

	if (isGoal_) {
		particle_->Update();
	}

}

void Goal::Draw(Camera* camera) {

	model_->Draw(camera);

}

void Goal::DrawParticle(Camera* camera) {

	if (isGoal_) {
		particle_->Draw(camera);
	}

}
