#include "Player.h"
#include <math.h>
#include <cmath>
#include "Engine/manager/ImGuiManager.h"

Player::Player()
{
}

Player::~Player()
{
}

void Player::Initialize() {

	input_ = Input::GetInstance();
	tex_ = TextureManager::GetInstance()->Load("./resources/white.png");
	sprite_.reset(Sprite::Create(tex_, { 100.0f,100.0f }));
	sprite_->size_ = { 100.0f,100.0f };
	speed_ = 5.0f;

}

void Player::Update() {

#ifdef _DEBUG

	ImGui::Begin("sprite");
	ImGui::DragFloat2("position", &sprite_->position_.x, 0.1f);
	ImGui::DragFloat2("size", &sprite_->size_.x, 0.1f);
	ImGui::End();

#endif // _DEBUG

	

}

void Player::Draw2D() {

	sprite_->Draw();

}

void Player::MoveLeft() {
	sprite_->position_.x -= speed_;
}

void Player::MoveRight() {
	sprite_->position_.x += speed_;
}
