#include "Warp.h"
#include "Rand.h"
#include "Game/ColorSetter/ColorSetter.h"
#include "Game/stage/Stage.h"
#include "ColorHolder.h"

Warp::Warp()
{
	modelA_.reset(Model::Create("./Resources/warp/warp.obj"));
	modelAWire_.reset(Model::Create("./Resources/warp/warp_wire.obj"));
	modelB_.reset(Model::Create("./Resources/warp/warp.obj"));
	modelBWire_.reset(Model::Create("./Resources/warp/warp_wire.obj"));
	colliderA_ = std::make_unique<BoxCollider>();
	colliderB_ = std::make_unique<BoxCollider>();
	lineBox_ = std::make_unique<LineBox>();
	lineBoxB_ = std::make_unique<LineBox>();
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
	colliderA_->SetFunction([this](Collider* collider) {OnCollisionA(collider); });
	lineBox_->SetOBB(&colliderA_->collider_);
	colliderB_->SetGameObject(this);
	colliderB_->collider_.center = transformB_->translate_;
	colliderB_->collider_.size = transformB_->scale_;
	colliderB_->SetFunction([this](Collider* collider) {OnCollisionB(collider); });
	lineBoxB_->SetOBB(&colliderB_->collider_);
	modelA_->SetColor(CreateColor(color_));
	modelB_->SetColor(CreateColor(color_));

}

void Warp::Update() {

	modelA_->material_->constMap_->edgeColor = CreateVector3Color(color_);
	modelB_->material_->constMap_->edgeColor = CreateVector3Color(color_);

	if (GameColor::IsActiveColor(color_, Stage::stageColor_, ColorHolder::GetHolderColor())) {
		
		if (modelA_->material_->constMap_->threshold > 0.0f) {

			modelA_->material_->constMap_->threshold -= 0.05f;

			if (modelA_->material_->constMap_->threshold < 0.0f) {
				modelA_->material_->constMap_->threshold = 0.0f;
			}

		}

		if (modelB_->material_->constMap_->threshold > 0.0f) {

			modelB_->material_->constMap_->threshold -= 0.05f;

			if (modelB_->material_->constMap_->threshold < 0.0f) {
				modelB_->material_->constMap_->threshold = 0.0f;
			}

		}
		
		colliderA_->SetIsActive(true);
		colliderB_->SetIsActive(true);
	}
	else {

		if (modelA_->material_->constMap_->threshold < 1.0f) {

			modelA_->material_->constMap_->threshold += 0.05f;

			if (modelA_->material_->constMap_->threshold > 1.0f) {
				modelA_->material_->constMap_->threshold = 1.0f;
			}

		}

		if (modelB_->material_->constMap_->threshold < 1.0f) {

			modelB_->material_->constMap_->threshold += 0.05f;

			if (modelB_->material_->constMap_->threshold > 1.0f) {
				modelB_->material_->constMap_->threshold = 1.0f;
			}

		}

		colliderA_->SetIsActive(false);
		colliderB_->SetIsActive(false);
	}

	isPreActiveWarp_ = isActiveWarp_;

	if (isPreActiveWarp_ and countCoolTimer_ > 0) {

		countCoolTimer_--;

	}

	isActiveWarp_ = true;

	if (countCoolTimer_ <= 0 and colliderA_->GetIsActive() and colliderB_->GetIsActive()) {

		transform_->rotate_.y += 0.02f;
		transformB_->rotate_.y += 0.02f;

		if (transform_->rotate_.y >= 6.24f) {
			transform_->rotate_.y = 0.0f;
		}

		if (transformB_->rotate_.y >= 6.24f) {
			transformB_->rotate_.y = 0.0f;
		}

		transform_->rotateQuaternion_ = ConvertFromEuler(transform_->rotate_);
		transformB_->rotateQuaternion_ = ConvertFromEuler(transformB_->rotate_);

	}

	transform_->UpdateMatrix();
	transformB_->UpdateMatrix();

	colliderA_->collider_.center = transform_->translate_;
	colliderA_->collider_.size = transform_->scale_;
	colliderB_->collider_.center = transformB_->translate_;
	colliderB_->collider_.size = transformB_->scale_;

	lineBox_->Update();
	lineBoxB_->Update();

	modelA_->SetWorldMatrix(transform_->worldMatrix_);
	modelAWire_->SetWorldMatrix(transform_->worldMatrix_);
	modelB_->SetWorldMatrix(transformB_->worldMatrix_);
	modelBWire_->SetWorldMatrix(transformB_->worldMatrix_);

	modelA_->SetColor(CreateColor(color_));
	modelAWire_->SetColor(CreateColor(color_));
	modelB_->SetColor(CreateColor(color_));
	modelBWire_->SetColor(CreateColor(color_));

}

void Warp::Draw(Camera* camera) {

	modelA_->Draw(camera);
	modelAWire_->Draw(camera);
	modelB_->Draw(camera);
	modelBWire_->Draw(camera);

}

void Warp::DrawLine(Camera* camera) {

#ifdef _DEBUG

	lineBox_->Draw(camera);
	lineBoxB_->Draw(camera);

#endif // _DEBUG

}

void Warp::OnCollisionA(Collider* collider) {

	//直前までワープが使われていなければ移動
	if (collider->GetGameObject()->GetName() == "player" || collider->GetGameObject()->GetName() == "copyCat") {

		if (isActiveWarp_ and isPreActiveWarp_ and countCoolTimer_ <= 0) {
			collider->GetGameObject()->SetPosition(transformB_->translate_);
		}

		isActiveWarp_ = false;
		countCoolTimer_ = coolTime_;

	}

}


void Warp::OnCollisionB(Collider* collider) {

	//直前までワープが使われていなければ移動
	if (collider->GetGameObject()->GetName() == "player" || collider->GetGameObject()->GetName() == "copyCat") {

		if (isActiveWarp_ and isPreActiveWarp_ and countCoolTimer_ <= 0) {
			collider->GetGameObject()->SetPosition(transform_->translate_);
		}

		isActiveWarp_ = false;
		countCoolTimer_ = coolTime_;

	}

}