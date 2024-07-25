#include "MoveBox.h"
#include "Rand.h"
#include "Game/stage/Stage.h"
#include "UsefulFunc.h"

MoveBox::MoveBox()
{
	model_.reset(Model::Create("./Resources/block/block.obj"));
	collider_ = std::make_unique<BoxCollider>();
	lineBox_ = std::make_unique<LineBox>();

}

MoveBox::~MoveBox()
{
}

void MoveBox::Initialize() {

	name_ = "moveBox";
	collider_->SetGameObject(this);
	collider_->collider_.center = transform_->translate_;
	collider_->collider_.size = transform_->scale_;
	preTranslate_ = collider_->collider_.center;
	collider_->SetFunction([this](Collider* collider) {OnCollision(collider); });
	lineBox_->SetOBB(&collider_->collider_);

}

void MoveBox::Update() {

	preTranslate_ = collider_->collider_.center;

	collider_->collider_.center = transform_->translate_;
	collider_->collider_.size = transform_->scale_;

	transform_->UpdateMatrix();

	lineBox_->Update();

	model_->SetWorldMatrix(transform_->worldMatrix_);

	model_->SetColor(CreateColor(colorNumber_));

}

void MoveBox::Draw(Camera* camera) {

	model_->Draw(camera);

}

void MoveBox::DrawLine(Camera* camera) {

	lineBox_->Draw(camera);

}

void MoveBox::OnCollision(Collider* collider) {

	if (collider->GetGameObject()->GetName() == "block" || collider->GetGameObject()->GetName() == "moveBox") {
		
		//上にいる時
		if (preTranslate_.y > collider->GetPosition().y + collider->GetSize().y) {

			//衝突したブロックの面積内なら上に留まる
			if (transform_->translate_.x - transform_->scale_.x * 0.5f < collider->GetPosition().x + collider->GetSize().x &&
				transform_->translate_.x + transform_->scale_.x * 0.5f > collider->GetPosition().x - collider->GetSize().x &&
				transform_->translate_.z - transform_->scale_.z * 0.5f < collider->GetPosition().z + collider->GetSize().z &&
				transform_->translate_.z + transform_->scale_.z * 0.5f > collider->GetPosition().z - collider->GetSize().z) {

				preTranslate_ = collider_->collider_.center;

				//当たり判定更新
				collider_->collider_.center.y = collider->GetPosition().y + collider->GetSize().y + collider_->collider_.size.y;

				//座標更新
				transform_->translate_.y = collider_->collider_.center.y;
				transform_->UpdateMatrix();

			}


		}
		//下にいる時
		else if (preTranslate_.y < collider->GetPosition().y - collider->GetSize().y) {

			//衝突したブロックの面積内なら押し出し
			if (transform_->translate_.x - transform_->scale_.x * 0.5f < collider->GetPosition().x + collider->GetSize().x &&
				transform_->translate_.x + transform_->scale_.x * 0.5f > collider->GetPosition().x - collider->GetSize().x &&
				transform_->translate_.z - transform_->scale_.z * 0.5f < collider->GetPosition().z + collider->GetSize().z &&
				transform_->translate_.z + transform_->scale_.z * 0.5f > collider->GetPosition().z - collider->GetSize().z) {

				preTranslate_ = collider_->collider_.center;

				//当たり判定更新
				collider_->collider_.center.y = collider->GetPosition().y - collider->GetSize().y - collider_->collider_.size.y;

				//座標更新
				transform_->translate_.y = collider_->collider_.center.y;
				transform_->UpdateMatrix();

			}

		}

		//左にいる時
		if (preTranslate_.x < collider->GetPosition().x - collider->GetSize().x &&
			preTranslate_.z + transform_->scale_.z > collider->GetPosition().z - collider->GetSize().z &&
			preTranslate_.z - transform_->scale_.z < collider->GetPosition().z + collider->GetSize().z) {

			//衝突したブロックの面積内なら押し出し
			if (transform_->translate_.y - transform_->scale_.y * 0.99f < collider->GetPosition().y + collider->GetSize().y &&
				transform_->translate_.y + transform_->scale_.y * 0.99f > collider->GetPosition().y - collider->GetSize().y &&
				transform_->translate_.z - transform_->scale_.z * 0.99f < collider->GetPosition().z + collider->GetSize().z &&
				transform_->translate_.z + transform_->scale_.z * 0.99f > collider->GetPosition().z - collider->GetSize().z) {

				preTranslate_ = collider_->collider_.center;

				//当たり判定更新
				collider_->collider_.center.x = collider->GetPosition().x - collider->GetSize().x - collider_->collider_.size.x;

				//座標更新
				transform_->translate_.x = collider_->collider_.center.x;
				transform_->UpdateMatrix();

			}

		}
		//右にいる時
		else if (preTranslate_.x > collider->GetPosition().x + collider->GetSize().x &&
			preTranslate_.z + transform_->scale_.z > collider->GetPosition().z - collider->GetSize().z &&
			preTranslate_.z - transform_->scale_.z < collider->GetPosition().z + collider->GetSize().z) {

			//衝突したブロックの面積内なら押し出し
			if (transform_->translate_.y - transform_->scale_.y * 0.99f < collider->GetPosition().y + collider->GetSize().y &&
				transform_->translate_.y + transform_->scale_.y * 0.99f > collider->GetPosition().y - collider->GetSize().y &&
				transform_->translate_.z - transform_->scale_.z * 0.99f < collider->GetPosition().z + collider->GetSize().z &&
				transform_->translate_.z + transform_->scale_.z * 0.99f > collider->GetPosition().z - collider->GetSize().z) {

				preTranslate_ = collider_->collider_.center;

				//当たり判定更新
				collider_->collider_.center.x = collider->GetPosition().x + collider->GetSize().x + collider_->collider_.size.x;

				//座標更新
				transform_->translate_.x = collider_->collider_.center.x;
				transform_->UpdateMatrix();

			}

		}

		//手前にいる時
		else if (preTranslate_.z < collider->GetPosition().z - collider->GetSize().z) {

			//衝突したブロックの面積内なら押し出し
			if (transform_->translate_.y - transform_->scale_.y * 0.99f < collider->GetPosition().y + collider->GetSize().y &&
				transform_->translate_.y + transform_->scale_.y * 0.99f > collider->GetPosition().y - collider->GetSize().y &&
				transform_->translate_.x - transform_->scale_.x * 0.99f < collider->GetPosition().x + collider->GetSize().x &&
				transform_->translate_.x + transform_->scale_.x * 0.99f > collider->GetPosition().x - collider->GetSize().x) {

				preTranslate_ = collider_->collider_.center;

				//当たり判定更新
				collider_->collider_.center.z = collider->GetPosition().z - collider->GetSize().z - collider_->collider_.size.z;

				//座標更新
				transform_->translate_.z = collider_->collider_.center.z;
				transform_->UpdateMatrix();

			}

		}
		//奥にいる時
		else if (preTranslate_.z > collider->GetPosition().z + collider->GetSize().z) {

			//衝突したブロックの面積内なら押し出し
			if (transform_->translate_.y - transform_->scale_.y * 0.99f < collider->GetPosition().y + collider->GetSize().y &&
				transform_->translate_.y + transform_->scale_.y * 0.99f > collider->GetPosition().y - collider->GetSize().y &&
				transform_->translate_.x - transform_->scale_.x * 0.99f < collider->GetPosition().x + collider->GetSize().x &&
				transform_->translate_.x + transform_->scale_.x * 0.99f > collider->GetPosition().x - collider->GetSize().x) {

				preTranslate_ = collider_->collider_.center;

				//当たり判定更新
				collider_->collider_.center.z = collider->GetPosition().z + collider->GetSize().z + collider_->collider_.size.z;

				//座標更新
				transform_->translate_.z = collider_->collider_.center.z;
				transform_->UpdateMatrix();

			}

		}

	}

}
