#include "Player.h"
#include <math.h>
#include <cmath>
#include "Drawing/ImGuiManager.h"
#include "Audio/AudioManager.h"
#include "PostEffectDrawer.h"
#include <functional>
#include "UsefulFunc.h"
#include "Game/stage/Stage.h"

Player::Player()
{

	tex_ = TextureManager::GetInstance()->Load("./Resources/plane/particle.png");
	model_.reset(SkinningModel::Create("./Resources/human/stay.gltf", 0));
	model_->LoadAnimation("./Resources/human/walking.gltf", 1);
	particle_.reset(Particle3D::Create("./Resources/plane/particle.obj", 128));
	particle_->SetInstanceCount(32);
	/*particle_->SetTexture(tex_);*/
	transform_ = std::make_unique<Transform>();
	collider_ = std::make_unique<BoxCollider>();

#ifdef _DEBUG

	lineBox_ = std::make_unique<LineBox>();

#endif // _DEBUG

}

Player::~Player()
{
}

void Player::Initialize() {

	input_ = Input::GetInstance();

	model_->ResetAnimation();
	model_->SetAnimation(0);
	model_->StartAnimation(true);
	model_->SetAnimationSpeed(1.0f);

	transform_->translate_ = { 0.0f,5.0f,0.0f };
	transform_->UpdateMatrix();
	model_->material_->pLightMap_->intensity = 2.0f;

	name_ = "player";
	collider_->SetGameObject(this);
	collider_->SetFunction([this](Collider* collider) {OnCollision(collider); });
	collider_->SetCollisionAttribute(0x00000001);
	collider_->SetCollisionMask(0xfffffffe);

	for (int32_t i = 0; i < 32; i++) {

		particle_->colors_[i] = CreateColor(Stage::stageColor_);
		particle_->velocities_[i] = { 0.0f,1.0f,0.0f };
		particle_->transforms_[i]->scale_ = { 0.0f,0.0f,0.0f };

	}

	collider_->collider_.center = transform_->translate_;
	collider_->collider_.size = { 0.5f,1.5f,0.5f };

	preTranslate_ = collider_->collider_.center;

	velocity_ = { 0.0f,0.0f,0.0f };

	isGoal_ = false;
	isDead_ = false;
	canJump_ = true;
	onGround_ = false;
	ringGetCount_ = 0;
	canGoal_ = false;

#ifdef _DEBUG

	lineBox_->SetOBB(&collider_->collider_);

#endif // _DEBUG


}

void Player::Update() {

	Vector3 moveVector{};

#ifdef _DEBUG

	ImGui::Begin("Player");
	ImGui::Text("velocity \nx : %1.3f \ny : %1.3f \nz : %1.3f", velocity_.x, velocity_.y, velocity_.z);
	ImGui::End();

	if (input_->PushKey(DIK_W)) {
		moveVector.z = 1.0f;
	}
	else if(input_->PushKey(DIK_S)) {
		moveVector.z = -1.0f;
	}

	if (input_->PushKey(DIK_A)) {
		moveVector.x = -1.0f;
	}
	else if (input_->PushKey(DIK_D)) {
		moveVector.x = 1.0f;
	}

#endif // _DEBUG

	//前フレーム当たり判定座標記録
	preTranslate_ = collider_->collider_.center;

	//死んでいない時の処理
	if (!isDead_) {

		//リングを規定数集めていたらゴール可能
		if (ringGetCount_ >= goalCount_) {
			canGoal_ = true;
		}
		else {
			canGoal_ = false;
		}

		

		if (fabsf(input_->GetStickValue(Input::LX)) > 0) {

			moveVector.x = input_->GetStickValue(Input::LX);

		}

		if (fabsf(input_->GetStickValue(Input::LY)) > 0) {

			moveVector.z = input_->GetStickValue(Input::LY);

		}

		if (fabsf(moveVector.x) + fabsf(moveVector.z) > 0.001f) {
			model_->SetAnimation(1, false);
		}
		else {
			model_->SetAnimation(0, false);
		}

		if (camera_) {
			moveVector = TransformNormal(Normalize(moveVector), camera_->matRotate_);
		}
		

		moveVector = Normalize(Vector3{ moveVector.x,0.0f,moveVector.z });

		moveVector *= speed_;

		Vector3 moveXZ = { moveVector.x, 0.0f, moveVector.z };

		//回転処理
		if (fabsf(Length(moveXZ)) > 0.00001f) {

			//一旦正規化
			moveXZ = Normalize(moveXZ);

			Vector3 tmp = ConjuGate(transform_->rotateQuaternion_) * moveXZ;

			Quaternion diff = MakeRotateAxisAngleQuaternion(Normalize(Cross({ 0.0f,0.0f,1.0f }, Normalize(tmp))), std::acos(Dot({ 0.0f,0.0f,1.0f }, Normalize(tmp))));

			transform_->rotateQuaternion_ = Slerp(IdentityQuaternion(), diff, 0.5f) * transform_->rotateQuaternion_;

		}

		//ジャンプ処理。地面にいたらジャンプ
		/*if (onGround_ && input_->TriggerButton(Input::Button::A)) {
			velocity_.y = jumpVelocity_;
			onGround_ = false;
		}*/

		velocity_.x = moveVector.x;
		velocity_.z = moveVector.z;

		velocity_.y -= 0.1f;

		if (velocity_.y < -2.0f) {
			velocity_.y = -2.0f;
		}

		//速度加算
		transform_->translate_ += velocity_;

		if (transform_->translate_.y < -20.0f) {
			isDead_ = true;
		}

		transform_->UpdateMatrix();

		collider_->collider_.center = transform_->worldMatrix_.GetTranslate() + Vector3{0.0f, collider_->collider_.size.y, 0.0f};

		model_->SetWorldMatrix(transform_->worldMatrix_);

		model_->UpdateAnimation();

		//パーティクル更新
		for (int32_t i = 0; i < 32; i++) {
			
			if (particle_->transforms_[i]->scale_.y <= 0.0f) {

				Matrix4x4 tmpMatrix{};

				tmpMatrix = model_->GetSkeletonSpaceMatrix("mixamorig:LeftHand") *
					model_->worldMatrix_;

				particle_->colors_[i] = CreateColor(Stage::stageColor_);
				particle_->velocities_[i] = { float((rand() % 40 - 20) * 0.001f),float((rand() % 40 - 20) * 0.001f), float((rand() % 40 - 20) * 0.001f) };
				particle_->transforms_[i]->translate_ = tmpMatrix.GetTranslate();
				particle_->transforms_[i]->rotateQuaternion_ = IdentityQuaternion();
				particle_->transforms_[i]->scale_ = { 0.1f,0.1f,0.1f };
				break;
			}



		}

		for (int32_t i = 0; i < 32; i++) {

			if (particle_->transforms_[i]->scale_.y > 0.0f) {
				particle_->colors_[i] = CreateColor(Stage::stageColor_);
				particle_->transforms_[i]->translate_ += particle_->velocities_[i];
				particle_->transforms_[i]->rotateQuaternion_ = particle_->transforms_[i]->rotateQuaternion_ * ConvertFromEuler(particle_->velocities_[i]);
				particle_->transforms_[i]->scale_ -= {0.002f, 0.002f, 0.002f};
			}
			else {
				particle_->colors_[i].w = 0.0f;
			}

		}

		//地面判定をfalseにする。ブロックとの判定時に切り替わる
		onGround_ = false;

	}

#ifdef _DEBUG

	lineBox_->Update();

#endif // _DEBUG


}

void Player::OnCollision(Collider* collider) {

	if (collider->GetGameObject()->GetName() == "block" || collider->GetGameObject()->GetName() == "moveBox" ||
		collider->GetGameObject()->GetName() == "ghostBox") {

		//上にいる時
		if (preTranslate_.y >= collider->GetPosition().y + collider->GetSize().y) {

			//衝突したブロックの面積内なら上に留まる
			if (transform_->translate_.x - transform_->scale_.x * 0.5f < collider->GetPosition().x + collider->GetSize().x &&
				transform_->translate_.x + transform_->scale_.x * 0.5f > collider->GetPosition().x - collider->GetSize().x &&
				transform_->translate_.z - transform_->scale_.z * 0.5f < collider->GetPosition().z + collider->GetSize().z &&
				transform_->translate_.z + transform_->scale_.z * 0.5f > collider->GetPosition().z - collider->GetSize().z) {

				preTranslate_ = collider_->collider_.center;

				//当たり判定更新
				collider_->collider_.center.y = collider->GetPosition().y + collider->GetSize().y + collider_->collider_.size.y;

				//座標更新
				transform_->translate_.y = collider_->collider_.center.y - collider_->collider_.size.y;
				transform_->UpdateMatrix();
				SetVelocityY(0.0f);
				onGround_ = true;

			}


		}
		//下にいる時
		else if (preTranslate_.y <= collider->GetPosition().y - collider->GetSize().y) {

			//衝突したブロックの面積内なら押し出し
			if (transform_->translate_.x - transform_->scale_.x * 0.5f < collider->GetPosition().x + collider->GetSize().x &&
				transform_->translate_.x + transform_->scale_.x * 0.5f > collider->GetPosition().x - collider->GetSize().x &&
				transform_->translate_.z - transform_->scale_.z * 0.5f < collider->GetPosition().z + collider->GetSize().z &&
				transform_->translate_.z + transform_->scale_.z * 0.5f > collider->GetPosition().z - collider->GetSize().z) {

				preTranslate_ = collider_->collider_.center;

				//当たり判定更新
				collider_->collider_.center.y = collider->GetPosition().y - collider->GetSize().y - collider_->collider_.size.y;

				//座標更新
				transform_->translate_.y = collider_->collider_.center.y - collider_->collider_.size.y;
				transform_->UpdateMatrix();

				if (velocity_.y > 0.0f) {
					SetVelocityY(0.0f);
				}

			}

		}

		//左にいる時
		if (preTranslate_.x <= collider->GetPosition().x - collider->GetSize().x &&
			preTranslate_.y < collider->GetPosition().y + collider->GetSize().y &&
			preTranslate_.y > collider->GetPosition().y - collider->GetSize().y) {

			//衝突したブロックの面積内なら押し出し
			if (transform_->translate_.y - transform_->scale_.y * 0.99f < collider->GetPosition().y + collider->GetSize().y &&
				transform_->translate_.y + transform_->scale_.y * 0.99f > collider->GetPosition().y - collider->GetSize().y &&
				transform_->translate_.z - transform_->scale_.z * 0.99f < collider->GetPosition().z + collider->GetSize().z &&
				transform_->translate_.z + transform_->scale_.z * 0.99f > collider->GetPosition().z - collider->GetSize().z) {

				if (collider->GetGameObject()->GetName() == "moveBox") {

					preTranslate_ = collider_->collider_.center;

					//当たり判定更新
					collider_->collider_.center.x = collider->GetPosition().x - collider->GetSize().x - collider_->collider_.size.x;

					//座標更新
					transform_->translate_.x = collider_->collider_.center.x;
					transform_->UpdateMatrix();

					collider->GetGameObject()->GetTransform()->translate_.x += velocity_.x;
					collider->SetPosition(collider->GetGameObject()->GetTransform()->translate_);

				}
				else {

					preTranslate_ = collider_->collider_.center;

					//当たり判定更新
					collider_->collider_.center.x = collider->GetPosition().x - collider->GetSize().x - collider_->collider_.size.x;

					//座標更新
					transform_->translate_.x = collider_->collider_.center.x;
					transform_->UpdateMatrix();

				}

			}

		}
		//右にいる時
		else if(preTranslate_.x >= collider->GetPosition().x + collider->GetSize().x &&
			preTranslate_.y < collider->GetPosition().y + collider->GetSize().y &&
			preTranslate_.y > collider->GetPosition().y - collider->GetSize().y) {

			//衝突したブロックの面積内なら押し出し
			if (transform_->translate_.y - transform_->scale_.y * 0.99f < collider->GetPosition().y + collider->GetSize().y &&
				transform_->translate_.y + transform_->scale_.y * 0.99f > collider->GetPosition().y - collider->GetSize().y &&
				transform_->translate_.z - transform_->scale_.z * 0.99f < collider->GetPosition().z + collider->GetSize().z &&
				transform_->translate_.z + transform_->scale_.z * 0.99f > collider->GetPosition().z - collider->GetSize().z) {

				if (collider->GetGameObject()->GetName() == "moveBox") {

					preTranslate_ = collider_->collider_.center;

					//当たり判定更新
					collider_->collider_.center.x = collider->GetPosition().x + collider->GetSize().x + collider_->collider_.size.x;

					//座標更新
					transform_->translate_.x = collider_->collider_.center.x;
					transform_->UpdateMatrix();

					collider->GetGameObject()->GetTransform()->translate_.x += velocity_.x;
					collider->SetPosition(collider->GetGameObject()->GetTransform()->translate_);

				}
				else {

					preTranslate_ = collider_->collider_.center;

					//当たり判定更新
					collider_->collider_.center.x = collider->GetPosition().x + collider->GetSize().x + collider_->collider_.size.x;

					//座標更新
					transform_->translate_.x = collider_->collider_.center.x;
					transform_->UpdateMatrix();

				}

			}

		}

		//手前にいる時
		if (preTranslate_.z <= collider->GetPosition().z - collider->GetSize().z &&
			preTranslate_.y < collider->GetPosition().y + collider->GetSize().y &&
			preTranslate_.y > collider->GetPosition().y - collider->GetSize().y) {

			//衝突したブロックの面積内なら押し出し
			if (transform_->translate_.y - transform_->scale_.y * 0.99f < collider->GetPosition().y + collider->GetSize().y &&
				transform_->translate_.y + transform_->scale_.y * 0.99f > collider->GetPosition().y - collider->GetSize().y &&
				transform_->translate_.x - transform_->scale_.x * 0.99f < collider->GetPosition().x + collider->GetSize().x &&
				transform_->translate_.x + transform_->scale_.x * 0.99f > collider->GetPosition().x - collider->GetSize().x) {

				if (collider->GetGameObject()->GetName() == "moveBox") {

					preTranslate_ = collider_->collider_.center;

					//当たり判定更新
					collider_->collider_.center.z = collider->GetPosition().z - collider->GetSize().z - collider_->collider_.size.z;

					//座標更新
					transform_->translate_.z = collider_->collider_.center.z;
					transform_->UpdateMatrix();

					collider->GetGameObject()->GetTransform()->translate_.z += velocity_.z;
					collider->SetPosition(collider->GetGameObject()->GetTransform()->translate_);

				}
				else {

					preTranslate_ = collider_->collider_.center;

					//当たり判定更新
					collider_->collider_.center.z = collider->GetPosition().z - collider->GetSize().z - collider_->collider_.size.z;

					//座標更新
					transform_->translate_.z = collider_->collider_.center.z;
					transform_->UpdateMatrix();

				}

			}

		}
		//奥にいる時
		else if (preTranslate_.z >= collider->GetPosition().z + collider->GetSize().z &&
			preTranslate_.y < collider->GetPosition().y + collider->GetSize().y &&
			preTranslate_.y > collider->GetPosition().y - collider->GetSize().y) {

			//衝突したブロックの面積内なら押し出し
			if (transform_->translate_.y - transform_->scale_.y * 0.99f < collider->GetPosition().y + collider->GetSize().y &&
				transform_->translate_.y + transform_->scale_.y * 0.99f > collider->GetPosition().y - collider->GetSize().y &&
				transform_->translate_.x - transform_->scale_.x * 0.99f < collider->GetPosition().x + collider->GetSize().x &&
				transform_->translate_.x + transform_->scale_.x * 0.99f > collider->GetPosition().x - collider->GetSize().x) {

				if (collider->GetGameObject()->GetName() == "moveBox") {

					preTranslate_ = collider_->collider_.center;

					//当たり判定更新
					collider_->collider_.center.z = collider->GetPosition().z + collider->GetSize().z + collider_->collider_.size.z;

					//座標更新
					transform_->translate_.z = collider_->collider_.center.z;
					transform_->UpdateMatrix();

					collider->GetGameObject()->GetTransform()->translate_.z += velocity_.z;
					collider->SetPosition(collider->GetGameObject()->GetTransform()->translate_);

				}
				else {

					preTranslate_ = collider_->collider_.center;

					//当たり判定更新
					collider_->collider_.center.z = collider->GetPosition().z + collider->GetSize().z + collider_->collider_.size.z;

					//座標更新
					transform_->translate_.z = collider_->collider_.center.z;
					transform_->UpdateMatrix();

				}

			}

		}

#ifdef _DEBUG
		//ライン更新
		lineBox_->Update();
#endif // _DEBUG

		//モデルに再度設定
		model_->SetWorldMatrix(transform_->worldMatrix_);

	}

	if (collider->GetGameObject()->GetName() == "ring") {
		ringGetCount_++;
	}

	//ゴールとの当たり判定
	if (collider->GetGameObject()->GetName() == "goal") {

		//ゴール可能な状態なら判定
		if (canGoal_) {
			isGoal_ = true;
		}

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

void Player::DrawLine(Camera* camera) {

#ifdef _DEBUG

	model_->DrawSkeleton(camera);

	lineBox_->Draw(camera);

#endif // _DEBUG

}
