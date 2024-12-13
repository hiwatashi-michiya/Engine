#include "CopyCat.h"
#include <math.h>
#include <cmath>
#include "Drawing/ImGuiManager.h"
#include "Audio/AudioManager.h"
#include "PostEffectDrawer.h"
#include <functional>
#include "Game/ColorSetter/ColorSetter.h"
#include "Game/stage/Stage.h"
#include "ColorHolder.h"

CopyCat::CopyCat()
{

	model_.reset(SkinningModel::Create("./Resources/player/brush_stay.gltf", 0));
	model_->LoadAnimation("./Resources/player/brush_walk.gltf", 1);
	transform_ = std::make_unique<Transform>();
	collider_ = std::make_unique<BoxCollider>();

#ifdef _DEBUG

	lineBox_ = std::make_unique<LineBox>();

#endif // _DEBUG

}

CopyCat::~CopyCat()
{
}

void CopyCat::Initialize() {

	input_ = Input::GetInstance();

	model_->ResetAnimation();
	model_->SetAnimation(0);
	model_->StartAnimation(true);
	model_->SetAnimationSpeed(2.0f);

	transform_->translate_ = { 0.0f,5.0f,0.0f };
	transform_->UpdateMatrix();
	model_->material_->pLightMap_->intensity = 2.0f;

	name_ = "copyCat";
	collider_->SetGameObject(this);
	collider_->SetFunction([this](Collider* collider) {OnCollision(collider); });
	collider_->SetCollisionAttribute(0x00000001);
	collider_->SetCollisionMask(0xfffffffe);

	collider_->collider_.center = transform_->translate_;
	collider_->collider_.size = { 0.5f,1.5f,0.5f };

	preTranslate_ = collider_->collider_.center;

	velocity_ = { 0.0f,0.0f,0.0f };

	isGoal_ = false;
	isDead_ = false;
	onGround_ = false;
	canGoal_ = false;

#ifdef _DEBUG

	lineBox_->SetOBB(&collider_->collider_);

#endif // _DEBUG


}

void CopyCat::Update() {

#ifdef _DEBUG

	

#endif // _DEBUG

#ifdef _DEBUG

	lineBox_->Update();

#endif // _DEBUG


}

void CopyCat::OnCollision([[maybe_unused]]Collider* collider) {


}

void CopyCat::Draw(Camera* camera) {

#ifdef _DEBUG

	model_->DrawSkeleton(camera);

	lineBox_->Draw(camera);

#endif // _DEBUG

	if (!isDead_) {
		model_->Draw(camera);
	}

}

