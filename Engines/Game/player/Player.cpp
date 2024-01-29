#include "Player.h"
#include <math.h>
#include <cmath>
#include "Engine/manager/ImGuiManager.h"

Player::Player()
{

	model_.reset(Model::Create("./resources/player/player.obj"));

	tex_ = TextureManager::GetInstance()->Load("./resources/goal/particle_2.png");

}

Player::~Player()
{
}

void Player::Initialize() {

	input_ = Input::GetInstance();

	model_->position_ = { 0.0f,0.0f,0.0f };

	particle_ = std::make_unique<Particle>();
	particle_->Initialize(Particle::k3D, Particle::kCircle);
	particle_->startPosition_ = model_->position_;
	particle_->AddParticle("./resources/plane/plane.obj", tex_, 10);
	particle_->SetColor({ 1.0f,1.0f,1.0f,1.0f });

}

void Player::Update() {

	if (moveCoolTimer_ > 0) {
		moveCoolTimer_--;
	}

	if (model_->scale_.x < 1.0f) {

		model_->scale_.x += 0.1f;

		if (model_->scale_.x > 1.0f) {
			model_->scale_.x = 1.0f;
		}

	}

	if (input_->PushButton(XINPUT_GAMEPAD_DPAD_UP) && moveCoolTimer_ == 0) {
		particle_->Reset();
		particle_->SetIsStart(true);
		model_->position_.z += moveVal_;
		moveCoolTimer_ = kMaxCoolTime_;
		model_->scale_.x = 0.6f;
		move_ = kUp;
		model_->rotation_.y = 3.14f;
	}
	else if (input_->PushButton(XINPUT_GAMEPAD_DPAD_DOWN) && moveCoolTimer_ == 0) {
		particle_->Reset();
		particle_->SetIsStart(true);
		model_->position_.z -= moveVal_;
		moveCoolTimer_ = kMaxCoolTime_;
		model_->scale_.x = 0.6f;
		move_ = kDown;
		model_->rotation_.y = 0.0f;
	}
	else if (input_->PushButton(XINPUT_GAMEPAD_DPAD_RIGHT) && moveCoolTimer_ == 0) {
		particle_->Reset();
		particle_->SetIsStart(true);
		model_->position_.x += moveVal_;
		moveCoolTimer_ = kMaxCoolTime_;
		model_->scale_.x = 0.6f;
		move_ = kRight;
		model_->rotation_.y = -1.57f;
	}
	else if (input_->PushButton(XINPUT_GAMEPAD_DPAD_LEFT) && moveCoolTimer_ == 0) {
		particle_->Reset();
		particle_->SetIsStart(true);
		model_->position_.x -= moveVal_;
		moveCoolTimer_ = kMaxCoolTime_;
		model_->scale_.x = 0.6f;
		move_ = kLeft;
		model_->rotation_.y = 1.57f;
	}

	if (!input_->PushButton(XINPUT_GAMEPAD_DPAD_UP) && !input_->PushButton(XINPUT_GAMEPAD_DPAD_DOWN) &&
		!input_->PushButton(XINPUT_GAMEPAD_DPAD_RIGHT) && !input_->PushButton(XINPUT_GAMEPAD_DPAD_LEFT)) {
		moveCoolTimer_ = 0;
	}

	collision_.max = model_->position_ + model_->scale_;
	collision_.min = model_->position_ - model_->scale_;

	particle_->startPosition_ = model_->position_;
	particle_->Update();

}

void Player::Draw(Camera* camera) {

	model_->Draw(camera);

}

void Player::DrawParticle(Camera* camera) {

	particle_->Draw(camera);

}
