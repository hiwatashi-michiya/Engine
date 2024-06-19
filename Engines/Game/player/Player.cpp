#include "Player.h"
#include <math.h>
#include <cmath>
#include "Drawing/ImGuiManager.h"
#include "Audio/AudioManager.h"
#include "PostEffectDrawer.h"
#include <functional>

Player::Player()
{

	tex_ = TextureManager::GetInstance()->Load("./Resources/plane/particle.png");
	model_.reset(SkinningModel::Create("./resources/human/stay.gltf", 0));
	model_->LoadAnimation("./resources/human/walking.gltf", 1);
	particle_.reset(Particle3D::Create("./Resources/plane/particle.obj", 128));
	particle_->SetInstanceCount(32);
	/*particle_->SetTexture(tex_);*/
	transform_ = std::make_unique<Transform>();
	collider_ = std::make_unique<BoxCollider>();

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

	name_ = "player";
	collider_->SetGameObject(this);
	collider_->SetFunction([this](Collider* collider) {OnCollision(collider); });

	for (int32_t i = 0; i < 32; i++) {

		particle_->colors_[i].w = 1.0f;
		particle_->velocities_[i] = { 0.0f,1.0f,0.0f };
		particle_->transforms_[i]->scale_ = { 0.0f,0.0f,0.0f };

	}

	collider_->collider_.center = transform_->translate_;
	collider_->collider_.size = transform_->scale_;

	/*collision_.max = transform_->translate_ + transform_->scale_;
	collision_.min = transform_->translate_ - transform_->scale_;*/

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

		collider_->collider_.center = transform_->translate_;

		/*collision_.max = transform_->translate_ + transform_->scale_;
		collision_.min = transform_->translate_ - transform_->scale_;*/

		transform_->UpdateMatrix();
		model_->SetWorldMatrix(transform_->worldMatrix_);

		model_->UpdateAnimation();

		//パーティクル更新
		for (int32_t i = 0; i < 32; i++) {
			
			if (particle_->transforms_[i]->scale_.y <= 0.0f) {

				Matrix4x4 tmpMatrix{};

				tmpMatrix = model_->GetSkeletonSpaceMatrix("mixamorig:LeftHand") *
					model_->worldMatrix_;

				particle_->colors_[i].w = 1.0f;
				particle_->velocities_[i] = { float((rand() % 40 - 20) * 0.001f),float((rand() % 40 - 20) * 0.001f), float((rand() % 40 - 20) * 0.001f) };
				particle_->transforms_[i]->translate_ = tmpMatrix.GetTranslate();
				particle_->transforms_[i]->rotateQuaternion_ = IdentityQuaternion();
				particle_->transforms_[i]->scale_ = { 0.1f,0.1f,0.1f };
				break;
			}



		}

		for (int32_t i = 0; i < 32; i++) {

			if (particle_->transforms_[i]->scale_.y > 0.0f) {
				particle_->transforms_[i]->translate_ += particle_->velocities_[i];
				particle_->transforms_[i]->rotateQuaternion_ = particle_->transforms_[i]->rotateQuaternion_ * ConvertFromEuler(particle_->velocities_[i]);
				particle_->transforms_[i]->scale_ -= {0.002f, 0.002f, 0.002f};
			}
			else {
				particle_->colors_[i].w = 0.0f;
			}

		}

	}

}

void Player::OnCollision(Collider* collider) {

	if (collider->GetGameObject()->GetName() == "block") {

		SetPosition({ transform_->worldMatrix_.GetTranslate().x, collider->GetGameObject()->GetTransform()->translate_.y +
					collider->GetGameObject()->GetTransform()->scale_.y + transform_->scale_.y, transform_->worldMatrix_.GetTranslate().z });
		SetVelocityY(0.0f);

	}

}

void Player::Draw(Camera* camera) {

	if (!isDead_) {
		model_->Draw(camera);
	}

}

void Player::DrawParticle(Camera* camera) {

	if (!isDead_) {
		particle_->Draw(camera);
	}

}

void Player::DrawSkeleton(Camera* camera) {

	model_->DrawSkeleton(camera);

}
