#include "Player.h"
#include <math.h>
#include <cmath>
#include "Drawing/ImGuiManager.h"
#include "Audio/AudioManager.h"
#include "PostEffectDrawer.h"

Player::Player()
{

	model_.reset(SkinningModel::Create("./resources/human/stay.gltf", 0));
	model_->LoadAnimation("./resources/human/walking.gltf", 1);
	transform_ = std::make_unique<Transform>();

}

Player::~Player()
{
}

void Player::Initialize() {

	input_ = Input::GetInstance();

	model_->ResetAnimation();
	model_->SetAnimation(0);
	model_->StartAnimation(true);
	model_->SetAnimationSpeed(2.0f);

	transform_->translate_ = { 0.0f,5.0f,0.0f };
	model_->material_->pLightMap_->intensity = 2.0f;

	collision_.max = transform_->translate_ + transform_->scale_;
	collision_.min = transform_->translate_ - transform_->scale_;

	isDead_ = false;

}

void Player::Update() {

#ifdef _DEBUG

	ImGui::Begin("Player");
	ImGui::Text("velocity \nx : %1.3f \ny : %1.3f \nz : %1.3f", velocity_.x, velocity_.y, velocity_.z);
	ImGui::End();

#endif // _DEBUG

	//死んでいない時の処理
	if (!isDead_) {

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

		if (fabsf(velocity_.x) + fabsf(velocity_.z) > 0.001f) {
			model_->SetAnimation(1, false);
		}
		else {
			model_->SetAnimation(0, false);
		}

		velocity_ = Normalize(velocity_);

		velocity_ *= speed_;

		Vector3 moveXZ = { velocity_.x, 0.0f, velocity_.z };

		//回転処理
		if (fabsf(Length(moveXZ)) > 0.00001f) {

			//一旦正規化
			moveXZ = Normalize(moveXZ);

			Vector3 tmp = ConjuGate(transform_->rotateQuaternion_) * moveXZ;

			Quaternion diff = MakeRotateAxisAngleQuaternion(Normalize(Cross({ 0.0f,0.0f,1.0f }, Normalize(tmp))), std::acos(Dot({ 0.0f,0.0f,1.0f }, Normalize(tmp))));

			transform_->rotateQuaternion_ = Slerp(IdentityQuaternion(), diff, 0.5f) * transform_->rotateQuaternion_;

		}

		velocity_.y -= 0.5f;

		//速度加算
		transform_->translate_ += velocity_;

		if (transform_->translate_.y < -20.0f) {
			isDead_ = true;
		}

		collision_.max = transform_->translate_ + transform_->scale_;
		collision_.min = transform_->translate_ - transform_->scale_;

		transform_->UpdateMatrix();
		model_->SetWorldMatrix(transform_->worldMatrix_);

		model_->UpdateAnimation();

	}

}

void Player::Draw(Camera* camera) {

	if (!isDead_) {
		model_->Draw(camera);
	}

}

void Player::DrawParticle(Camera* camera) {

}

void Player::DrawSkeleton(Camera* camera) {

	model_->DrawSkeleton(camera);

}
