#include "FollowCamera.h"
#include "Input.h"

FollowCamera::FollowCamera()
{
}

FollowCamera::~FollowCamera()
{
}

void FollowCamera::Update() {

	if (camera_) {

		if (target_) {

			Vector2 addRotate = { -float(Input::GetInstance()->GetStickValue(Input::Stick::RY) * 0.000001f),
			float(Input::GetInstance()->GetStickValue(Input::Stick::RX) * 0.000001f) };

			camera_->rotation_ += {addRotate.x, addRotate.y, 0.0f};

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
