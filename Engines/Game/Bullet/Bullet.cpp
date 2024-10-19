#include "Bullet.h"
#include "Game/stage/Stage.h"

PlayerBullet::PlayerBullet()
{

	model_.reset(Model::Create("./Resources/bullet/bullet.obj"));
	transform_ = std::make_unique<Transform>();
	collider_ = std::make_unique<BoxCollider>();

}

PlayerBullet::~PlayerBullet()
{
}

void PlayerBullet::Initialize(const Vector3& velocity, const Vector3& startPosition, float lifeTime)
{

	velocity_ = velocity;
	transform_->translate_ = startPosition;
	lifeTime_ = lifeTime;
	bulletColor_ = Stage::stageColor_;
	model_->SetColor(GameColor::CreateColor(bulletColor_));

	name_ = "P_Bullet";
	collider_->collider_.center = transform_->translate_;
	collider_->collider_.size = transform_->scale_;
	collider_->SetGameObject(this);
	collider_->SetFunction([this](Collider* collider) {OnCollision(collider); });
	collider_->SetCollisionAttribute(0x00000001);
	collider_->SetCollisionMask(0xfffffffe);

}

void PlayerBullet::Update()
{

	//生存時間中は移動
	if (lifeTime_ > 0) {

		lifeTime_--;
		transform_->translate_ += velocity_;

	}
	else {

		isDead_ = true;
		collider_->SetIsActive(false);

	}

	transform_->UpdateMatrix();
	model_->SetWorldMatrix(transform_->worldMatrix_);

	collider_->collider_.center = transform_->translate_;

}

void PlayerBullet::Draw(Camera* camera)
{

	model_->Draw(camera);

}

void PlayerBullet::OnCollision(Collider* collider)
{

	lifeTime_ = 0;
	isDead_ = true;

}
