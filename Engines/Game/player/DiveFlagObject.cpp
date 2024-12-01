#include "DiveFlagObject.h"
#include "Player.h"
#include "Game/Gimmick/GhostBox.h"

DiveFlagObject::DiveFlagObject()
{
	transform_ = std::make_unique<Transform>();
	collider_ = std::make_unique<BoxCollider>();
#ifdef _DEBUG

	lineBox_ = std::make_unique<LineBox>();

#endif // _DEBUG
}

DiveFlagObject::~DiveFlagObject()
{
}

void DiveFlagObject::Initialize(Transform* parent, Player* player)
{

	player_ = player;

	//プレイヤーのトランスフォームを親として、そこから正面方向に向けて当たり判定を移動
	transform_->SetParent(parent);
	transform_->translate_ = { 0.0f,2.0f,1.0f };
	transform_->UpdateMatrix();
	collider_->collider_.center = transform_->worldMatrix_.GetTranslate();
	collider_->collider_.size = { 0.011f,0.011f,0.011f };

	//コライダーのセット
	name_ = "P_Dive";
	collider_->SetGameObject(this);
	collider_->SetFunction([this](Collider* collider) {OnCollision(collider); });
	collider_->SetCollisionAttribute(0x00000001);
	collider_->SetCollisionMask(0xfffffffe);

#ifdef _DEBUG

	lineBox_->SetOBB(&collider_->collider_);

#endif // _DEBUG

}

void DiveFlagObject::Update()
{

	transform_->UpdateMatrix();
	collider_->collider_.center = transform_->worldMatrix_.GetTranslate();

#ifdef _DEBUG

	lineBox_->Update();

#endif // _DEBUG

}

void DiveFlagObject::Draw(Camera* camera)
{
	//ライン描画
#ifdef _DEBUG

	lineBox_->Draw(camera);

#endif // _DEBUG
}

void DiveFlagObject::OnCollision(Collider* collider)
{

	if (collider->GetGameObject()->GetName() == "ghostBox") {

		//プレイヤーがあるとき且つ停止か移動状態の時
		if (player_ and
			(player_->behavior_ == Player::Behavior::kStay or player_->behavior_ == Player::Behavior::kMove)) {

#ifdef _DEBUG

			//ボタンをトリガーしていたら潜る状態に移行
			if (player_->input_->TriggerKey(DIK_Q)) {
				//ブロックの回転向きを取得
				auto blockPtr = dynamic_cast<GhostBox*>(collider->GetGameObject());
				//プレイヤーの回転向きにセット
				player_->playerDive_->rotateType_ = blockPtr->GetRotateType();
				//状態を切り替える
				player_->behaviorRequest_ = Player::Behavior::kDive;
			}

#endif // _DEBUG


			//ボタンをトリガーしていたら潜る状態に移行
			if (player_->input_->TriggerButton(Input::Button::LB)) {
				//ブロックの回転向きを取得
				auto blockPtr = dynamic_cast<GhostBox*>(collider->GetGameObject());
				//プレイヤーの回転向きにセット
				player_->playerDive_->rotateType_ = blockPtr->GetRotateType();
				//状態を切り替える
				player_->behaviorRequest_ = Player::Behavior::kDive;
			}

		}

	}

}
