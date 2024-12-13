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

#ifdef _DEBUG

			if (not isSwitching_ and Input::GetInstance()->TriggerKey(DIK_C) and canSwitch_) {
				isSwitching_ = true;
			}

#endif // _DEBUG

			//切り替え可能で、現在切り替えていない時にYボタンを押したら
			if (not isSwitching_ and Input::GetInstance()->TriggerButton(Input::Button::Y) and canSwitch_) {
				isSwitching_ = true;
			}

			//切り替え終わりのフラグを降ろす
			if (isSwitched_) {
				isSwitched_ = false;
			}

			//切り替え途中
			if (isSwitching_) {

				float t = 0.0f;

				if (easingCount_ != 0) {
					t = Easing::InSine(float(easingCount_) / float(maxEasingTime_));
				}

				camera_->rotation_ = Lerp(sideAngle_, aboveAngle_, t);

				//横から視点の場合
				if (cameraType_ == CommonVariables::CameraType::kSide) {

					//カウント加算
					easingCount_++;
					//カウントが最大値になったらカメラのタイプ変更、切り替え処理終了
					if (easingCount_ >= maxEasingTime_) {
						camera_->rotation_ = aboveAngle_;
						cameraType_ = CommonVariables::CameraType::kAbove;
						easingCount_ = maxEasingTime_;
						isSwitching_ = false;
						isSwitched_ = true;
					}

				}
				//上から視点の場合
				else {

					//カウント加算
					easingCount_--;
					//カウントが最大値になったらカメラのタイプ変更、切り替え処理終了
					if (easingCount_ <= 0) {
						camera_->rotation_ = sideAngle_;
						cameraType_ = CommonVariables::CameraType::kSide;
						easingCount_ = 0;
						isSwitching_ = false;
						isSwitched_ = true;
					}

				}

			}
			else {

				//タイプに応じてカメラの回転固定
				if (cameraType_ == CommonVariables::CameraType::kSide) {
					camera_->rotation_ = sideAngle_;
				}
				else {
					camera_->rotation_ = aboveAngle_;
				}

			}
			

			/*Vector2 addRotate = { -float(Input::GetInstance()->GetStickValue(Input::Stick::RY) * 0.000001f),
			float(Input::GetInstance()->GetStickValue(Input::Stick::RX) * 0.000001f) };

			camera_->rotation_ += {addRotate.x, addRotate.y, 0.0f};*/

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
