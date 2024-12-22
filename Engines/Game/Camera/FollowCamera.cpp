#include "FollowCamera.h"
#include "Input.h"
#include <cmath>
#include <algorithm>
#include "Easing.h"

CommonVariables::CameraType FollowCamera::cameraType_ = CommonVariables::CameraType::kSide;

FollowCamera::FollowCamera()
{
}

FollowCamera::~FollowCamera()
{
}

void FollowCamera::Initialize()
{
	cameraType_ = CommonVariables::CameraType::kSide;
}

void FollowCamera::Update() {

	if (camera_) {

		if (target_) {
			
			//カメラの回転速度
			float deltaSpeed = 0.000001f;
			//スティック入力でカメラ回転
			Vector2 addRotate = { -float(Input::GetInstance()->GetStickValue(Input::Stick::RY) * deltaSpeed),
			float(Input::GetInstance()->GetStickValue(Input::Stick::RX) * deltaSpeed) };

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
