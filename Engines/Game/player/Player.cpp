#include "Player.h"
#include <math.h>
#include <cmath>
#include "Drawing/ImGuiManager.h"
#include "Audio/AudioManager.h"
#include "PostEffectDrawer.h"
#include <functional>
#include "UsefulFunc.h"
#include "Game/stage/Stage.h"
#include "Game/Gimmick/GhostBox.h"
#include <numbers>
#include "Game/Camera/FollowCamera.h"
#include "ParticleManager.h"

Player::Player()
{

	tex_ = TextureManager::GetInstance()->Load("./Resources/particle/particle.png");
	model_.reset(SkinningModel::Create("./Resources/player/brush_stay.gltf", 0));
	model_->LoadAnimation("./Resources/player/brush_walk.gltf", 1);
	model_->LoadAnimation("./Resources/player/brush_shot.gltf", 2);
	model_->LoadAnimation("./Resources/player/brush_dive.gltf", 3);

	if (ParticleManager::GetInstance()->GetUnUsedParticle(particle_)) {

		particle_->Load("./Resources/ParticleData/player.json");

	}

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
	model_->SetAnimationSpeed(2.0f);

	transform_->translate_ = { 0.0f,5.0f,0.0f };
	transform_->UpdateMatrix();
	model_->material_->pLightMap_->intensity = 2.0f;

	name_ = "player";
	collider_->SetGameObject(this);
	collider_->SetFunction([this](Collider* collider) {OnCollision(collider); });
	collider_->SetCollisionAttribute(0x00000001);
	collider_->SetCollisionMask(0xfffffffe);

	particle_->SetColor(CreateColor(Stage::stageColor_));

	collider_->collider_.center = transform_->translate_;
	collider_->collider_.size = { 1.0f,1.0f,1.0f };

	preTranslate_ = collider_->collider_.center;

	velocity_ = { 0.0f,0.0f,0.0f };

	isGoal_ = false;
	isDead_ = false;
	onGround_ = false;
	itemGetCount_ = 0;
	canGoal_ = false;

	//停止状態で初期化
	state_ = std::make_unique<PlayerStay>();
	state_->Initialize(this);

#ifdef _DEBUG

	lineBox_->SetOBB(&collider_->collider_);

#endif // _DEBUG


}

void Player::Update() {

	Vector3 moveVector{};

	//デバッグ用入力処理
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
	if (not isDead_) {

		state_->Update();

		//速度の下限を超えないように調整
		if (velocity_.y < fallSpeedLimit_) {
			velocity_.y = fallSpeedLimit_;
		}

		//速度加算
		transform_->translate_ += velocity_;

		//デッドラインまで落ちたら死亡
		if (transform_->translate_.y < deadHeight_) {
			isDead_ = true;
			//死亡パーティクル付ける

		}

		//行列更新
		transform_->UpdateMatrix();
		//当たり判定更新
		collider_->collider_.center = transform_->worldMatrix_.GetTranslate() + Vector3{0.0f, collider_->collider_.size.y, 0.0f};
		//モデルに更新した行列をセット
		model_->SetWorldMatrix(transform_->worldMatrix_);
		//モデルのアニメーション更新
		model_->UpdateAnimation();

		//半分の値
		float half = 0.5f;

		particle_->SetMinMaxSpawnPoint(transform_->translate_ - transform_->scale_ * half, transform_->translate_ + transform_->scale_ * half);
		particle_->SetColor(CreateColor(Stage::stageColor_));

		particle_->Update();
		

		//地面判定をfalseにする。ブロックとの判定時に切り替わる
		onGround_ = false;
	}

	groundPosition_ = nullptr;
	groundSize_ = nullptr;

#ifdef _DEBUG

	lineBox_->Update();

#endif // _DEBUG


}

void Player::CorrectionPosition()
{

	//ポインタに値が入っている時(地面に触れている時)補正可能にする
	if (groundPosition_ and groundSize_) {

		//以前の座標の保存
		preTranslate_ = collider_->collider_.center;

		//中央にずらす
		collider_->collider_.center.z = groundPosition_->z;

		//座標更新
		transform_->translate_.z = collider_->collider_.center.z;
		transform_->UpdateMatrix();

	}

}

void Player::SetAnimation(int32_t number, bool flag, float speed)
{

	model_->SetAnimation(number, flag);
	model_->SetAnimationSpeed(speed);

}

void Player::SetPlayerState(std::unique_ptr<PlayerState> nextState)
{
	//過去の状態を開放してから新しい状態を挿入
	state_.release();
	state_ = std::move(nextState);
	//初期化
	state_->Initialize(this);
}

void Player::OnCollision(Collider* collider) {

	if (collider->GetGameObject()->GetName() == "block" or collider->GetGameObject()->GetName() == "moveBox" or
		collider->GetGameObject()->GetName() == "ghostBox") {

		//潜る状態の時
		if (isDiving_) {

			if (auto pBox = dynamic_cast<GhostBox*>(collider->GetGameObject())) {
				
				//センターがブロック内に埋まっていたら
				if (IsCollision(pBox->GetCollider()->collider_, collider_->collider_.center)) {
					//カウントをセット
					diveCount_ = poseDiveCount_;
				}

				return;

			}

		}

		//上にいる時
		if (preTranslate_.y >= collider->GetPosition().y + collider->GetSize().y) {

			//衝突したブロックの面積内なら上に留まる
			if (transform_->translate_.x - transform_->scale_.x * kHeightCorrectValue_ < collider->GetPosition().x + collider->GetSize().x and
				transform_->translate_.x + transform_->scale_.x * kHeightCorrectValue_ > collider->GetPosition().x - collider->GetSize().x and
				transform_->translate_.z - transform_->scale_.z * kHeightCorrectValue_ < collider->GetPosition().z + collider->GetSize().z and
				transform_->translate_.z + transform_->scale_.z * kHeightCorrectValue_ > collider->GetPosition().z - collider->GetSize().z) {

				preTranslate_ = collider_->collider_.center;

				//当たり判定更新
				collider_->collider_.center.y = collider->GetPosition().y + collider->GetSize().y + collider_->collider_.size.y;

				//座標更新
				transform_->translate_.y = collider_->collider_.center.y - collider_->collider_.size.y;
				transform_->UpdateMatrix();

				//潜る状態のとき
				if (isDiving_) {

					//面の反対側から向かってきている場合
					if (velocity_.y < -kSpeedBorder_ * speed_) {
						//velocityを180度回転
						RotateVelocity();
					}

				}
				else {

					SetVelocityY(0.0f);
					onGround_ = true;
				}

			}


		}
		//下にいる時
		else if (preTranslate_.y <= collider->GetPosition().y - collider->GetSize().y) {

			//衝突したブロックの面積内なら押し出し
			if (transform_->translate_.x - transform_->scale_.x * kHeightCorrectValue_ < collider->GetPosition().x + collider->GetSize().x and
				transform_->translate_.x + transform_->scale_.x * kHeightCorrectValue_ > collider->GetPosition().x - collider->GetSize().x and
				transform_->translate_.z - transform_->scale_.z * kHeightCorrectValue_ < collider->GetPosition().z + collider->GetSize().z and
				transform_->translate_.z + transform_->scale_.z * kHeightCorrectValue_ > collider->GetPosition().z - collider->GetSize().z) {

				preTranslate_ = collider_->collider_.center;

				//当たり判定更新
				collider_->collider_.center.y = collider->GetPosition().y - collider->GetSize().y - collider_->collider_.size.y;

				//座標更新
				transform_->translate_.y = collider_->collider_.center.y - collider_->collider_.size.y;
				transform_->UpdateMatrix();

				//潜る状態のとき
				if (isDiving_) {

					//面の反対側から向かってきている場合
					if (velocity_.y > kSpeedBorder_ * speed_) {
						//velocityを180度回転
						RotateVelocity();
					}

				}
				else {
					if (velocity_.y > 0.0f) {
						SetVelocityY(0.0f);
					}
				}

			}

		}

		//左にいる時
		if (preTranslate_.x <= collider->GetPosition().x - collider->GetSize().x and
			preTranslate_.y < collider->GetPosition().y + collider->GetSize().y and
			preTranslate_.y > collider->GetPosition().y - collider->GetSize().y) {

			//衝突したブロックの面積内なら押し出し
			if (transform_->translate_.y - transform_->scale_.y * kWidthCorrectValue_ < collider->GetPosition().y + collider->GetSize().y and
				transform_->translate_.y + transform_->scale_.y * kWidthCorrectValue_ > collider->GetPosition().y - collider->GetSize().y and
				transform_->translate_.z - transform_->scale_.z * kWidthCorrectValue_ < collider->GetPosition().z + collider->GetSize().z and
				transform_->translate_.z + transform_->scale_.z * kWidthCorrectValue_ > collider->GetPosition().z - collider->GetSize().z) {

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

					//潜る状態のとき
					if (isDiving_) {

						//面の反対側から向かってきている場合
						if (velocity_.x > kSpeedBorder_ * speed_) {
							//velocityを180度回転
							RotateVelocity();
						}

					}

				}

			}

		}
		//右にいる時
		else if(preTranslate_.x >= collider->GetPosition().x + collider->GetSize().x and
			preTranslate_.y < collider->GetPosition().y + collider->GetSize().y and
			preTranslate_.y > collider->GetPosition().y - collider->GetSize().y) {

			//衝突したブロックの面積内なら押し出し
			if (transform_->translate_.y - transform_->scale_.y * kWidthCorrectValue_ < collider->GetPosition().y + collider->GetSize().y and
				transform_->translate_.y + transform_->scale_.y * kWidthCorrectValue_ > collider->GetPosition().y - collider->GetSize().y and
				transform_->translate_.z - transform_->scale_.z * kWidthCorrectValue_ < collider->GetPosition().z + collider->GetSize().z and
				transform_->translate_.z + transform_->scale_.z * kWidthCorrectValue_ > collider->GetPosition().z - collider->GetSize().z) {

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

					//潜る状態のとき
					if (isDiving_) {

						//面の反対側から向かってきている場合
						if (velocity_.x < -kSpeedBorder_ * speed_) {
							//velocityを180度回転
							RotateVelocity();
						}

					}

				}

			}

		}

		//手前にいる時
		if (preTranslate_.z <= collider->GetPosition().z - collider->GetSize().z and
			preTranslate_.y < collider->GetPosition().y + collider->GetSize().y and
			preTranslate_.y > collider->GetPosition().y - collider->GetSize().y) {

			//衝突したブロックの面積内なら押し出し
			if (transform_->translate_.y - transform_->scale_.y * kWidthCorrectValue_ < collider->GetPosition().y + collider->GetSize().y and
				transform_->translate_.y + transform_->scale_.y * kWidthCorrectValue_ > collider->GetPosition().y - collider->GetSize().y and
				transform_->translate_.x - transform_->scale_.x * kWidthCorrectValue_ < collider->GetPosition().x + collider->GetSize().x and
				transform_->translate_.x + transform_->scale_.x * kWidthCorrectValue_ > collider->GetPosition().x - collider->GetSize().x) {

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

					//潜る状態のとき
					if (isDiving_) {

						//面の反対側から向かってきている場合
						if (velocity_.z > kSpeedBorder_ * speed_) {
							//velocityを180度回転
							RotateVelocity();
						}

					}

				}

			}

		}
		//奥にいる時
		else if (preTranslate_.z >= collider->GetPosition().z + collider->GetSize().z and
			preTranslate_.y < collider->GetPosition().y + collider->GetSize().y and
			preTranslate_.y > collider->GetPosition().y - collider->GetSize().y) {

			//衝突したブロックの面積内なら押し出し
			if (transform_->translate_.y - transform_->scale_.y * kWidthCorrectValue_ < collider->GetPosition().y + collider->GetSize().y and
				transform_->translate_.y + transform_->scale_.y * kWidthCorrectValue_ > collider->GetPosition().y - collider->GetSize().y and
				transform_->translate_.x - transform_->scale_.x * kWidthCorrectValue_ < collider->GetPosition().x + collider->GetSize().x and
				transform_->translate_.x + transform_->scale_.x * kWidthCorrectValue_ > collider->GetPosition().x - collider->GetSize().x) {

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

					//潜る状態のとき
					if (isDiving_) {

						//面の反対側から向かってきている場合
						if (velocity_.z < -kSpeedBorder_ * speed_) {
							//velocityを180度回転
							RotateVelocity();
						}

					}

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
		itemGetCount_++;
	}

}

void Player::RotateVelocity()
{

	velocity_ = RotateOnYAxis(velocity_, float(std::numbers::pi));

	//プレイヤーの向きを更新
	transform_->rotateQuaternion_ =
		RotateForAxis(transform_->rotateQuaternion_, Vector3::AxisZ(), velocity_, 1.0f);

	//速度の下限
	float speedBorderValue = 0.0001f;

	//velocityの誤差を無くすため、ここで調整
	if (fabsf(velocity_.x) < speedBorderValue) {
		velocity_.x = 0.0f;
	}

	if (fabsf(velocity_.y) < speedBorderValue) {
		velocity_.y = 0.0f;
	}

	if (fabsf(velocity_.z) < speedBorderValue) {
		velocity_.z = 0.0f;
	}

}

void Player::Draw(Camera* camera)
{

	//ライン描画
#ifdef _DEBUG

	model_->DrawSkeleton(camera);

	lineBox_->Draw(camera);

#endif // _DEBUG

	//モデル描画
	if (not isDead_) {
		model_->Draw(camera);
	}
	//パーティクル描画
	if (not isDead_) {
		particle_->Draw(camera);
	}

}
