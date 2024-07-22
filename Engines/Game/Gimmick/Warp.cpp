#include "Warp.h"
#include "Rand.h"

Warp::Warp()
{
	modelA_.reset(Model::Create("./Resources/block/block.obj"));
	modelB_.reset(Model::Create("./Resources/block/block.obj"));
	colliderA_ = std::make_unique<BoxCollider>();
	colliderB_ = std::make_unique<BoxCollider>();
	lineBox_ = std::make_unique<LineBox>();
	transformB_ = std::make_unique<Transform>();

}

Warp::~Warp()
{
}

void Warp::Initialize() {

	name_ = "warp";
	colliderA_->SetGameObject(this);
	colliderA_->collider_.center = transform_->translate_;
	colliderA_->collider_.size = transform_->scale_;
	preTranslate_ = colliderA_->collider_.center;
	colliderA_->SetFunction([this](Collider* collider) {OnCollision(collider); });
	lineBox_->SetOBB(&colliderA_->collider_);

}

void Warp::Update() {

	preTranslate_ = colliderA_->collider_.center;

	colliderA_->collider_.center = transform_->translate_;
	colliderA_->collider_.size = transform_->scale_;

	transform_->UpdateMatrix();

	lineBox_->Update();

	modelA_->SetWorldMatrix(transform_->worldMatrix_);

}

void Warp::Draw(Camera* camera) {

	modelA_->Draw(camera);

}

void Warp::DrawLine(Camera* camera) {

	lineBox_->Draw(camera);

}

void Warp::OnCollision(Collider* collider) {

	

}
