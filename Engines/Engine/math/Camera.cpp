#include "Camera.h"

void Camera::Initialize() {
	//行列の初期化
	matProjection_ = MakePerspectiveFovMatrix(0.45f, float(1280.0f) / float(720.0f), 0.1f, 500.0f);

	matWorld_ = MakeScaleMatrix(scale_) * matRotate_ * MakeTranslateMatrix(position_);

	matView_ = Inverse(matWorld_);

}

void Camera::Update() {
	//回転更新
	rotateQuaternion_ = ConvertFromEuler(rotation_);
	matRotate_ = MakeRotateMatrix(rotation_);
	//行列更新
	matWorld_ = MakeScaleMatrix(scale_) * matRotate_ * MakeTranslateMatrix(position_);
	matView_ = Inverse(matWorld_);
	matViewProjection_ = matView_ * matProjection_;

}
