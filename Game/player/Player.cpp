#include "Player.h"
#include "Engine/input/Input.h"

Player::Player()
{
}

Player::~Player()
{
}

void Player::Initialize() {

	playerModel_.reset(Model::Create("./resources/cube/cube.obj"));

}

void Player::Update() {

}

void Player::Draw() {

	playerModel_->Draw();

}
