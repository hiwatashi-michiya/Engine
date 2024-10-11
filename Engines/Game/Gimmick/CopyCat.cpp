#include "CopyCat.h"
#include <math.h>
#include <cmath>
#include "Drawing/ImGuiManager.h"
#include "Audio/AudioManager.h"
#include "PostEffectDrawer.h"
#include <functional>
#include "UsefulFunc.h"
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
	canJump_ = true;
	onGround_ = false;
	ringGetCount_ = 0;
	canGoal_ = false;

#ifdef _DEBUG

	lineBox_->SetOBB(&collider_->collider_);

#endif // _DEBUG


}

void CopyCat::Update() {

#ifdef _DEBUG

	

#endif // _DEBUG

	//前フレーム当たり判定座標記録
	preTranslate_ = collider_->collider_.center;

	//死んでいない時の処理
	if (!isDead_) {

		//リングを規定数集めていたらゴール可能
		/*if (ringGetCount_ >= goalCount_) {
			canGoal_ = true;
		}
		else {
			canGoal_ = false;
		}*/

		if (player_ && (colorNumber_ == Stage::stageColor_ || colorNumber_ == ColorHolder::GetHolderColor() ||
			colorNumber_ == -1)) {

			model_->StartAnimation(true);

			//回転処理
			transform_->rotateQuaternion_ = player_->GetRotate();

			velocity_.x = player_->GetVelocity().x;
			velocity_.z = player_->GetVelocity().z;

		}
		else {

			model_->ResetAnimation();
			model_->StartAnimation(false);

			velocity_ = { 0.0f,0.0f,0.0f };

		}

		if (fabsf(velocity_.x) + fabsf(velocity_.z) > 0.001f) {
			model_->SetAnimation(1, false);
		}
		else {
			model_->SetAnimation(0, false);
		}

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

		collider_->collider_.center = transform_->worldMatrix_.GetTranslate() + Vector3{ 0.0f, collider_->collider_.size.y, 0.0f };

		model_->SetWorldMatrix(transform_->worldMatrix_);

		model_->UpdateAnimation();

		if (colorNumber_ != -1) {
			model_->SetColor(CreateColor(colorNumber_));
		}
		else {
			model_->SetColor({ 5.0f,5.0f,5.0f,5.0f });
		}

		//地面判定をfalseにする。ブロックとの判定時に切り替わる
		onGround_ = false;

	}
	else if (isDead_) {

		velocity_ = { 0.0f,0.0f,0.0f };
		transform_->translate_ = respawnPosition_;
		transform_->UpdateMatrix();
		isDead_ = false;

	}

#ifdef _DEBUG

	lineBox_->Update();

#endif // _DEBUG


}

void CopyCat::OnCollision(Collider* collider) {

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
		else if (preTranslate_.x >= collider->GetPosition().x + collider->GetSize().x &&
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

	if (collider->GetGameObject()->GetName() == "paint") {
		
		if (player_) {
			player_->AddRingCount();
		}

	}

	if (collider->GetGameObject()->GetName() == "enemy") {

		isDead_ = true;

	}

	//ゴールとの当たり判定
	//if (collider->GetGameObject()->GetName() == "goal") {

	//	//ゴール可能な状態なら判定
	//	if (canGoal_) {
	//		isGoal_ = true;
	//	}

	//}

}

void CopyCat::Draw(Camera* camera) {

	if (!isDead_) {
		model_->Draw(camera);
	}

}

void CopyCat::DrawLine(Camera* camera) {

#ifdef _DEBUG

	model_->DrawSkeleton(camera);

	lineBox_->Draw(camera);

#endif // _DEBUG

}
