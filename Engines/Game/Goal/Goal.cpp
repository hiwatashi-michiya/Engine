#include "Goal.h"
#include "Game/stage/Stage.h"
#include "UsefulFunc.h"

Goal::Goal()
{
	model_.reset(Model::Create("./Resources/goal/goal.obj"));
	particle_.reset(Particle3D::Create("./Resources/particle/particle.obj", kMaxParticles_));
	particle_->SetInstanceCount(particleCount_);
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

}

void Goal::Update() {

	//エフェクトが非アクティブの時
	if (not isActiveEffect_ and player_ and player_->GetCanGoal()) {
		isActiveEffect_ = true;
	}

	collider_->collider_.center = transform_->translate_;
	collider_->collider_.size = transform_->scale_;

	transform_->rotate_.y += changeValue_;

	if (transform_->rotate_.y > kMaxRotate_) {
		transform_->rotate_.y = 0.0f;
	}

	transform_->rotateQuaternion_ = ConvertFromEuler(transform_->rotate_);

	transform_->UpdateMatrix();

	lineBox_->Update();

	model_->SetWorldMatrix(transform_->worldMatrix_);

	if (isActiveEffect_) {
		model_->SetColor(CreateColor(Stage::stageColor_));
	}
	else {
		model_->SetColor({ 1.0f,1.0f,1.0f,1.0f });
	}

}

void Goal::Draw(Camera* camera) {

#ifdef _DEBUG

	lineBox_->Draw(camera);

#endif // _DEBUG

	

	model_->Draw(camera);
}
