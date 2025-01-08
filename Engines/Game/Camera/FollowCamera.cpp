#include "FollowCamera.h"
#include "Input.h"
#include <cmath>
#include <algorithm>
#include "Easing.h"

FollowCamera::FollowCamera()
{
}

FollowCamera::~FollowCamera()
{
}

void FollowCamera::Initialize()
{

	if (camera_) {
		//カメラアングル初期化
		camera_->rotation_ = startAngle_;
	}

}

void FollowCamera::Update() {

	if (camera_) {

		if (target_) {
			
			//デバッグ用
#ifdef _DEBUG

			float rotateSpeed = 0.05f;

			if (Input::GetInstance()->PushKey(DIK_UP)) {
				camera_->rotation_.x += rotateSpeed;
			}
			else if (Input::GetInstance()->PushKey(DIK_DOWN)) {
				camera_->rotation_.x -= rotateSpeed;
			}

			if (Input::GetInstance()->PushKey(DIK_LEFT)) {
				camera_->rotation_.y -= rotateSpeed;
			}
			if (Input::GetInstance()->PushKey(DIK_RIGHT)) {
				camera_->rotation_.y += rotateSpeed;
			}

#endif // _DEBUG


			//カメラの回転速度
			float deltaSpeed = 0.000001f;
			//スティック入力でカメラ回転
			Vector2 addRotate = { -float(Input::GetInstance()->GetStickValue(Input::Stick::RY) * deltaSpeed),
			float(Input::GetInstance()->GetStickValue(Input::Stick::RX) * deltaSpeed) };

			camera_->rotation_ += {addRotate.x, addRotate.y, 0.0f};

			camera_->rotation_.x = std::clamp(camera_->rotation_.x, 0.6f, 1.2f);

			camera_->matRotate_ = MakeRotateMatrix(camera_->rotation_);

			Vector3 offsetPosition = offset_;

			//オフセットをカメラの回転に合わせて傾ける
			offsetPosition = TransformNormal(offsetPosition, camera_->matRotate_);

			//傾けたオフセット位置をターゲットの座標に足して、カメラの位置とする
			camera_->position_ = target_->worldMatrix_.GetTranslate() + offsetPosition;

		}

		camera_->Update();

	}

}
