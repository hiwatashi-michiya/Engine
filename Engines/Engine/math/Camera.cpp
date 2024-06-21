#include "Camera.h"

void Camera::Initialize() {

	matProjection_ = MakePerspectiveFovMatrix(0.45f, float(1280.0f) / float(720.0f), 0.1f, 500.0f);

	matWorld_ = MakeScaleMatrix(scale_) * matRotate_ * MakeTranslateMatrix(position_);

	matView_ = Inverse(matWorld_);

}

void Camera::Update() {

	rotateQuaternion_ = ConvertFromEuler(rotation_);
	matRotate_ = MakeRotateMatrix(rotation_);

	matWorld_ = MakeScaleMatrix(scale_) * matRotate_ * MakeTranslateMatrix(position_);

	matView_ = Inverse(matWorld_);

	matViewProjection_ = matView_ * matProjection_;

}
