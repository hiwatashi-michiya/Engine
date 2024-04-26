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

	collision_.max = transform_->translate_ + transform_->scale_;
	collision_.min = transform_->translate_ - transform_->scale_;

}

void Player::Update() {

#ifdef _DEBUG

	ImGui::Begin("Player");
	ImGui::Text("velocity \nx : %1.3f \ny : %1.3f \nz : %1.3f", velocity_.x, velocity_.y, velocity_.z);
	ImGui::End();

#endif // _DEBUG


	if (fabsf(input_->GetStickValue(Input::LX)) > 0) {

		velocity_.x = input_->GetStickValue(Input::LX);

	}
	else {
		velocity_.x = 0.0f;
	}

	if (fabsf(input_->GetStickValue(Input::LY)) > 0) {

		velocity_.z = input_->GetStickValue(Input::LY);

	}
	else {
		velocity_.z = 0.0f;
	}

	velocity_ = Normalize(velocity_);

	velocity_.y -= 0.5f;

	velocity_ *= speed_;

	Vector3 moveXZ = { velocity_.x, 0.0f, velocity_.z };

	//回転処理
	if (fabsf(Length(moveXZ)) > 0.00001f) {

		//一旦正規化
		moveXZ = Normalize(moveXZ);

		Vector3 tmp = ConjuGate(transform_->rotateQuaternion_) * moveXZ;

		Quaternion diff = MakeRotateAxisAngleQuaternion(Normalize(Cross({0.0f,0.0f,1.0f}, Normalize(tmp))), std::acos(Dot({0.0f,0.0f,1.0f}, Normalize(tmp))));

		transform_->rotateQuaternion_ = Slerp(IdentityQuaternion(), diff, 0.5f) * transform_->rotateQuaternion_;

	}

	//速度加算
	transform_->translate_ += velocity_;

	if (transform_->translate_.y < -30.0f) {
		transform_->translate_ = { 0.0f,10.0f,0.0f };
	}

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
