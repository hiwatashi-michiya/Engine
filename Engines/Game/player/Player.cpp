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

	tex_ = TextureManager::GetInstance()->Load("./Resources/particle/particle.png");
	model_.reset(SkinningModel::Create("./Resources/player/brush_stay.gltf", 0));
	model_->LoadAnimation("./Resources/player/brush_walk.gltf", 1);
	model_->LoadAnimation("./Resources/player/brush_shot.gltf", 2);

	particle_ = std::make_shared<Particle>();
	particle_->Initialize();
	particle_->Load("./Resources/ParticleData/player.json");

	/*particle_->SetTexture(tex_);*/
	transform_ = std::make_unique<Transform>();
	collider_ = std::make_unique<BoxCollider>();
	playerStay_ = std::make_unique<PlayerStay>();
	playerMove_ = std::make_unique<PlayerMove>();
	playerShot_ = std::make_unique<PlayerShot>();

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
	collider_->collider_.size = { 0.5f,1.5f,0.5f };

	preTranslate_ = collider_->collider_.center;

	velocity_ = { 0.0f,0.0f,0.0f };

	isGoal_ = false;
	isDead_ = false;
	onGround_ = false;
	itemGetCount_ = 0;
	canGoal_ = false;

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

	//弾が生存していない場合は削除
	bullets_.erase(std::remove_if(bullets_.begin(), bullets_.end(), [](auto bullet) {

		if (bullet->GetIsDead()) {
			return true;
		}

		return false;

		}), bullets_.end());

	//死んでいない時の処理
	if (!isDead_) {

		//リングを規定数集めていたらゴール可能
		if (itemGetCount_ >= goalCount_) {
			canGoal_ = true;
		}
		else {
			canGoal_ = false;
		}

		//切り替えの受付があったら状態遷移
		if (behaviorRequest_) {

			behavior_ = behaviorRequest_.value();

			//状態に応じて初期化処理を変更
			switch (behavior_)
			{
			default:
			case Player::Behavior::kStay:
				playerStay_->InitializeStay(*this);
				break;
			case Player::Behavior::kMove:
				playerMove_->InitializeMove(*this);
				break;
			case Player::Behavior::kShot:
				playerShot_->InitializeShot(*this);
				break;
			}

			behaviorRequest_ = std::nullopt;

		}

		//状態に応じた更新処理
		switch (behavior_)
		{
		default:
		case Player::Behavior::kStay:
			playerStay_->UpdateStay(*this);
			break;
		case Player::Behavior::kMove:
			playerMove_->UpdateMove(*this);
			break;
		case Player::Behavior::kShot:
			playerShot_->UpdateShot(*this);
			break;
		}

		//落下速度更新
		velocity_.y += fallSpeed_;

		if (velocity_.y < fallSpeedLimit_) {
			velocity_.y = fallSpeedLimit_;
		}

		//速度加算
		transform_->translate_ += velocity_;

		//デッドラインまで落ちたら死亡
		if (transform_->translate_.y < deadHeight_) {
			isDead_ = true;
		}

		//行列更新
		transform_->UpdateMatrix();
		//当たり判定更新
		collider_->collider_.center = transform_->worldMatrix_.GetTranslate() + Vector3{0.0f, collider_->collider_.size.y, 0.0f};
		//モデルに更新した行列をセット
		model_->SetWorldMatrix(transform_->worldMatrix_);
		//モデルのアニメーション更新
		model_->UpdateAnimation();

		//弾更新
		for (int32_t i = 0; i < bullets_.size(); i++) {
			bullets_[i]->Update();
		}

		particle_->SetMinMaxSpawnPoint(transform_->translate_ - transform_->scale_ * 0.5f, transform_->translate_ + transform_->scale_ * 0.5f);
		particle_->SetColor(CreateColor(Stage::stageColor_));

		particle_->Update();
		

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
			if (transform_->translate_.x - transform_->scale_.x * 0.5f < collider->GetPosition().x + collider->GetSize().x and
				transform_->translate_.x + transform_->scale_.x * 0.5f > collider->GetPosition().x - collider->GetSize().x and
				transform_->translate_.z - transform_->scale_.z * 0.5f < collider->GetPosition().z + collider->GetSize().z and
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
			if (transform_->translate_.x - transform_->scale_.x * 0.5f < collider->GetPosition().x + collider->GetSize().x and
				transform_->translate_.x + transform_->scale_.x * 0.5f > collider->GetPosition().x - collider->GetSize().x and
				transform_->translate_.z - transform_->scale_.z * 0.5f < collider->GetPosition().z + collider->GetSize().z and
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
		if (preTranslate_.x <= collider->GetPosition().x - collider->GetSize().x and
			preTranslate_.y < collider->GetPosition().y + collider->GetSize().y and
			preTranslate_.y > collider->GetPosition().y - collider->GetSize().y) {

			//衝突したブロックの面積内なら押し出し
			if (transform_->translate_.y - transform_->scale_.y * 0.99f < collider->GetPosition().y + collider->GetSize().y and
				transform_->translate_.y + transform_->scale_.y * 0.99f > collider->GetPosition().y - collider->GetSize().y and
				transform_->translate_.z - transform_->scale_.z * 0.99f < collider->GetPosition().z + collider->GetSize().z and
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
		else if(preTranslate_.x >= collider->GetPosition().x + collider->GetSize().x and
			preTranslate_.y < collider->GetPosition().y + collider->GetSize().y and
			preTranslate_.y > collider->GetPosition().y - collider->GetSize().y) {

			//衝突したブロックの面積内なら押し出し
			if (transform_->translate_.y - transform_->scale_.y * 0.99f < collider->GetPosition().y + collider->GetSize().y and
				transform_->translate_.y + transform_->scale_.y * 0.99f > collider->GetPosition().y - collider->GetSize().y and
				transform_->translate_.z - transform_->scale_.z * 0.99f < collider->GetPosition().z + collider->GetSize().z and
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
		if (preTranslate_.z <= collider->GetPosition().z - collider->GetSize().z and
			preTranslate_.y < collider->GetPosition().y + collider->GetSize().y and
			preTranslate_.y > collider->GetPosition().y - collider->GetSize().y) {

			//衝突したブロックの面積内なら押し出し
			if (transform_->translate_.y - transform_->scale_.y * 0.99f < collider->GetPosition().y + collider->GetSize().y and
				transform_->translate_.y + transform_->scale_.y * 0.99f > collider->GetPosition().y - collider->GetSize().y and
				transform_->translate_.x - transform_->scale_.x * 0.99f < collider->GetPosition().x + collider->GetSize().x and
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
		else if (preTranslate_.z >= collider->GetPosition().z + collider->GetSize().z and
			preTranslate_.y < collider->GetPosition().y + collider->GetSize().y and
			preTranslate_.y > collider->GetPosition().y - collider->GetSize().y) {

			//衝突したブロックの面積内なら押し出し
			if (transform_->translate_.y - transform_->scale_.y * 0.99f < collider->GetPosition().y + collider->GetSize().y and
				transform_->translate_.y + transform_->scale_.y * 0.99f > collider->GetPosition().y - collider->GetSize().y and
				transform_->translate_.x - transform_->scale_.x * 0.99f < collider->GetPosition().x + collider->GetSize().x and
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
		itemGetCount_++;
	}

	if (collider->GetGameObject()->GetName() == "enemy") {

		isDead_ = true;

	}

	//ゴールとの当たり判定
	if (collider->GetGameObject()->GetName() == "goal") {

		//ゴール可能な状態なら判定
		if (canGoal_) {
			isGoal_ = true;
		}

	}

}

void Player::Draw(Camera* camera)
{

	//ライン描画
#ifdef _DEBUG

	model_->DrawSkeleton(camera);

	lineBox_->Draw(camera);

#endif // _DEBUG

	//弾描画
	for (int32_t i = 0; i < bullets_.size(); i++) {
		bullets_[i]->Draw(camera);
	}
	//モデル描画
	if (not isDead_) {
		model_->Draw(camera);
	}
	//パーティクル描画
	if (not isDead_) {
		particle_->Draw(camera);
	}

}
