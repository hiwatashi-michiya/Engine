#include "Block.h"
#include "Rand.h"
#include "Game/stage/Stage.h"

Block::Block()
{
	model_ = std::make_unique<Model>();
	model_->Initialize("./Resources/block/block.obj");
	collider_ = std::make_unique<BoxCollider>();
	lineBox_ = std::make_unique<LineBox>();

}

Block::~Block()
{
}

void Block::Initialize() {

	name_ = "block";
	collider_->SetGameObject(this);
	collider_->collider_.center = transform_->translate_;
	collider_->collider_.size = transform_->scale_;
	collider_->SetCollisionAttribute(0x00000002);
	collider_->SetCollisionMask(0xfffffffd);
	collider_->SetFunction([this](Collider* collider) {OnCollision(collider); });
	lineBox_->SetOBB(&collider_->collider_);

}

void Block::Update() {

	collider_->collider_.center = transform_->translate_;
	collider_->collider_.size = transform_->scale_;

	transform_->UpdateMatrix();

	lineBox_->Update();

	model_->SetColor(CreateColor(color_));
	model_->SetWorldMatrix(transform_->worldMatrix_);

}

void Block::Draw(Camera* camera) {

	model_->Draw(camera);
	lineBox_->Draw(camera);

}


void Block::OnCollision([[maybe_unused]]Collider* collider)
{

}

