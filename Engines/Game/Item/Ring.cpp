#include "Ring.h"
#include "Audio/AudioManager.h"
#include "Game/stage/Stage.h"
#include "UsefulFunc.h"

Ring::Ring()
{
	model_.reset(Model::Create("./Resources/item/item.gltf"));
	collider_ = std::make_unique<BoxCollider>();
	lineBox_ = std::make_unique<LineBox>();
}

Ring::~Ring()
{
}

void Ring::Initialize(const Vector3& position) {

	name_ = "ring";
	model_->LoadAnimation("./Resources/item/item.gltf");
	model_->SetMesh("./Resources/item/wireRing.gltf");
	transform_->translate_ = position;

	collider_->SetGameObject(this);
	collider_->collider_.center = transform_->translate_;
	collider_->collider_.size = transform_->scale_;
	collider_->SetIsActive(true);
	collider_->SetFunction([this](Collider* collider) {OnCollision(collider); });
	lineBox_->SetOBB(&collider_->collider_);

	model_->StartAnimation(true);

	getSE_ = AudioManager::GetInstance()->LoadInMF("./Resources/SE/ring_get.mp3");

}

void Ring::Obtained() {

	isObtained_ = true;
	model_->LoadAnimation("./Resources/item/item_2.gltf");
	model_->ResetAnimation();
	model_->SetIsLoop(false);
	model_->SetAnimationSpeed(5.0f);
	collider_->SetIsActive(false);
	PlaySE();

}

void Ring::PlaySE() {

	AudioManager::GetInstance()->Play(getSE_, 0.8f);

}

void Ring::Update() {

	model_->UpdateAnimation();

	//ゲットした演出アニメーションを終えたら破壊
	if (isObtained_ && model_->IsEndAnimation()) {

		transform_->scale_ = Vector3::Zero();

		isVanish_ = true;

	}

	//色が揃っていて取得されていない時
	if (colorNumber_ == Stage::stageColor_ && !isObtained_) {
		model_->SetMesh("./Resources/item/item.gltf");
		collider_->SetIsActive(true);
	}
	//取得された時
	else if (isObtained_) {
		model_->SetMesh("./Resources/item/item.gltf");
		collider_->SetIsActive(false);
	}
	//色が揃っていない時
	else {
		model_->SetMesh("./Resources/item/wireRing.gltf");
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

	model_->SetColor(CreateColor(colorNumber_));

}

void Ring::Draw(Camera* camera) {

	model_->Draw(camera);

}

void Ring::DrawLine(Camera* camera) {

#ifdef _DEBUG

	lineBox_->Draw(camera);

#endif // _DEBUG

}

void Ring::OnCollision(Collider* collider) {

	if (collider->GetGameObject()->GetName() == "player") {
		Obtained();
	}

}
