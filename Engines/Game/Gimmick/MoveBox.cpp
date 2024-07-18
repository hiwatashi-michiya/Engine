#include "MoveBox.h"
#include "Rand.h"

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

	setCount_ = (rand() % 10) * 5 + 10;

	name_ = "moveBox";
	collider_->SetGameObject(this);
	collider_->collider_.center = transform_->translate_;
	collider_->collider_.size = transform_->scale_;
	preTranslate_ = collider_->collider_.center;
	collider_->SetFunction([this](Collider* collider) {OnCollision(collider); });
	/*collider_->SetCollisionAttribute(0x00000003);
	collider_->SetCollisionMask(0xfffffffc);*/
	lineBox_->SetOBB(&collider_->collider_);

}

void MoveBox::Update() {

	preTranslate_ = collider_->collider_.center;

	//光源の点滅
	if (lightCount_ <= 0) {

		if (pLightIntensity_ > 0.5f) {

			pLightIntensity_ -= 0.01f;

			if (pLightIntensity_ < 0.5f) {
				pLightIntensity_ = 0.5f;
			}

		}

	}
	else if (lightCount_ > 0) {

		if (pLightIntensity_ < 1.0f) {

			pLightIntensity_ += 0.01f;

			if (pLightIntensity_ > 1.0f) {
				pLightIntensity_ = 1.0f;
			}

		}

		lightCount_--;

	}

	//ランダムで明かりを点滅させる
	if (--setCount_ <= 0) {

		int32_t random = rand() % 33;

		if (random == 0) {
			lightCount_ = 30;
		}

		setCount_ = (rand() % 10) * 5 + 10;

	}

	collider_->collider_.center = transform_->translate_;
	collider_->collider_.size = transform_->scale_;

	model_->material_->pLightMap_->intensity = pLightIntensity_;

	transform_->UpdateMatrix();

	lineBox_->Update();

	model_->SetWorldMatrix(transform_->worldMatrix_);

}

void MoveBox::Draw(Camera* camera) {

	model_->Draw(camera);

}

void MoveBox::DrawLine(Camera* camera) {

	lineBox_->Draw(camera);

}

void MoveBox::OnCollision(Collider* collider) {

	if (collider->GetGameObject()->GetName() == "block") {
		
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
		if (preTranslate_.x < collider->GetPosition().x - collider->GetSize().x) {

			//衝突したブロックの面積内なら押し出し
			if (transform_->translate_.y - transform_->scale_.y * 0.5f < collider->GetPosition().y + collider->GetSize().y &&
				transform_->translate_.y + transform_->scale_.y * 0.5f > collider->GetPosition().y - collider->GetSize().y &&
				transform_->translate_.z - transform_->scale_.z * 0.5f < collider->GetPosition().z + collider->GetSize().z &&
				transform_->translate_.z + transform_->scale_.z * 0.5f > collider->GetPosition().z - collider->GetSize().z) {

				if (collider->GetGameObject()->GetName() == "block") {

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
		else if (preTranslate_.x > collider->GetPosition().x + collider->GetSize().x) {

			//衝突したブロックの面積内なら押し出し
			if (transform_->translate_.y - transform_->scale_.y * 0.5f < collider->GetPosition().y + collider->GetSize().y &&
				transform_->translate_.y + transform_->scale_.y * 0.5f > collider->GetPosition().y - collider->GetSize().y &&
				transform_->translate_.z - transform_->scale_.z * 0.5f < collider->GetPosition().z + collider->GetSize().z &&
				transform_->translate_.z + transform_->scale_.z * 0.5f > collider->GetPosition().z - collider->GetSize().z) {

				if (collider->GetGameObject()->GetName() == "block") {

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
		if (preTranslate_.z < collider->GetPosition().z - collider->GetSize().z) {

			//衝突したブロックの面積内なら押し出し
			if (transform_->translate_.y - transform_->scale_.y * 0.5f < collider->GetPosition().y + collider->GetSize().y &&
				transform_->translate_.y + transform_->scale_.y * 0.5f > collider->GetPosition().y - collider->GetSize().y &&
				transform_->translate_.x - transform_->scale_.x * 0.5f < collider->GetPosition().x + collider->GetSize().x &&
				transform_->translate_.x + transform_->scale_.x * 0.5f > collider->GetPosition().x - collider->GetSize().x) {

				if (collider->GetGameObject()->GetName() == "block") {

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
		else if (preTranslate_.z > collider->GetPosition().z + collider->GetSize().z) {

			//衝突したブロックの面積内なら押し出し
			if (transform_->translate_.y - transform_->scale_.y * 0.5f < collider->GetPosition().y + collider->GetSize().y &&
				transform_->translate_.y + transform_->scale_.y * 0.5f > collider->GetPosition().y - collider->GetSize().y &&
				transform_->translate_.x - transform_->scale_.x * 0.5f < collider->GetPosition().x + collider->GetSize().x &&
				transform_->translate_.x + transform_->scale_.x * 0.5f > collider->GetPosition().x - collider->GetSize().x) {

				if (collider->GetGameObject()->GetName() == "block") {

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

}
