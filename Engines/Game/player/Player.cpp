#include "Player.h"
#include <math.h>
#include <cmath>
#include "Drawing/ImGuiManager.h"
#include "Audio/AudioManager.h"

Player::Player()
{

	model_.reset(Model::Create("./resources/player/player.obj"));
	transform_ = std::make_unique<Transform>();
	tex_ = TextureManager::GetInstance()->Load("./resources/goal/particle_2.png");

}

Player::~Player()
{
}

void Player::Initialize() {

	input_ = Input::GetInstance();

	transform_->translate_ = { 0.0f,0.0f,0.0f };
	model_->material_->pLightMap_->intensity = 2.0f;

	particle_ = std::make_unique<Particle>();
	particle_->Initialize(Particle::k3D, Particle::kCircle);
	particle_->startPosition_ = transform_->translate_;
	particle_->AddParticle("./resources/plane/plane.obj", tex_, 10);
	particle_->SetColor({ 1.0f,1.0f,1.0f,1.0f });

	moveSE_ = AudioManager::GetInstance()->SoundLoadWave("./resources/SE/move.wav");

}

void Player::Update() {

	if (moveCoolTimer_ > 0) {
		moveCoolTimer_--;
	}

	if (++animationTime_ >= 30) {
		animationTime_ = 0;
		transform_->scale_.y = 0.6f;
	}

	if (transform_->scale_.x < 1.0f) {

		transform_->scale_.x += 0.1f;

		if (transform_->scale_.x > 1.0f) {
			transform_->scale_.x = 1.0f;
		}

	}

	if (transform_->scale_.y < 1.0f) {

		transform_->scale_.y += 0.1f;

		if (transform_->scale_.y > 1.0f) {
			transform_->scale_.y = 1.0f;
		}

	}

	if (input_->PushButton(XINPUT_GAMEPAD_DPAD_UP) && moveCoolTimer_ == 0) {
		particle_->Reset();
		particle_->SetIsStart(true);
		transform_->translate_.z += moveVal_;
		moveCoolTimer_ = kMaxCoolTime_;
		transform_->scale_.x = 0.6f;
		move_ = kUp;
		transform_->rotate_.y = 0.0f;
		AudioManager::GetInstance()->Play(moveSE_, 0.5f);
	}
	else if (input_->PushButton(XINPUT_GAMEPAD_DPAD_DOWN) && moveCoolTimer_ == 0) {
		particle_->Reset();
		particle_->SetIsStart(true);
		transform_->translate_.z -= moveVal_;
		moveCoolTimer_ = kMaxCoolTime_;
		transform_->scale_.x = 0.6f;
		move_ = kDown;
		transform_->rotate_.y = 3.14f;
		AudioManager::GetInstance()->Play(moveSE_, 0.5f);
	}
	else if (input_->PushButton(XINPUT_GAMEPAD_DPAD_RIGHT) && moveCoolTimer_ == 0) {
		particle_->Reset();
		particle_->SetIsStart(true);
		transform_->translate_.x += moveVal_;
		moveCoolTimer_ = kMaxCoolTime_;
		transform_->scale_.x = 0.6f;
		move_ = kRight;
		transform_->rotate_.y = 1.57f;
		AudioManager::GetInstance()->Play(moveSE_, 0.5f);
	}
	else if (input_->PushButton(XINPUT_GAMEPAD_DPAD_LEFT) && moveCoolTimer_ == 0) {
		particle_->Reset();
		particle_->SetIsStart(true);
		transform_->translate_.x -= moveVal_;
		moveCoolTimer_ = kMaxCoolTime_;
		transform_->scale_.x = 0.6f;
		move_ = kLeft;
		transform_->rotate_.y = -1.57f;
		AudioManager::GetInstance()->Play(moveSE_, 0.5f);
	}

	if (!input_->PushButton(XINPUT_GAMEPAD_DPAD_UP) && !input_->PushButton(XINPUT_GAMEPAD_DPAD_DOWN) &&
		!input_->PushButton(XINPUT_GAMEPAD_DPAD_RIGHT) && !input_->PushButton(XINPUT_GAMEPAD_DPAD_LEFT)) {
		moveCoolTimer_ = 0;
	}

	collision_.max = transform_->translate_ + transform_->scale_;
	collision_.min = transform_->translate_ - transform_->scale_;

	particle_->startPosition_ = transform_->translate_;
	particle_->Update();

	transform_->UpdateMatrix();
	model_->SetWorldMatrix(transform_->worldMatrix_);

}

void Player::Draw(Camera* camera) {

	model_->Draw(camera);

}

void Player::DrawParticle(Camera* camera) {

	particle_->Draw(camera);

}
