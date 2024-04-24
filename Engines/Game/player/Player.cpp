#include "Player.h"
#include <math.h>
#include <cmath>
#include "Drawing/ImGuiManager.h"
#include "Audio/AudioManager.h"

Player::Player()
{

	model_.reset(Model::Create("./resources/player/player.obj"));
	transform_ = std::make_unique<Transform>();

}

Player::~Player()
{
}

void Player::Initialize() {

	input_ = Input::GetInstance();

	transform_->translate_ = { 0.0f,0.0f,0.0f };
	model_->material_->pLightMap_->intensity = 2.0f;

}

void Player::Update() {



	collision_.max = transform_->translate_ + transform_->scale_;
	collision_.min = transform_->translate_ - transform_->scale_;

	transform_->UpdateMatrix();
	model_->SetWorldMatrix(transform_->worldMatrix_);

}

void Player::Draw(Camera* camera) {

	model_->Draw(camera);

}

void Player::DrawParticle(Camera* camera) {

}
