#include "WorldTransform.h"

WorldTransform::WorldTransform()
{
	matWorld_ = MakeAffineMatrix(scale_, rotation_, translation_);
	rotateMatrix_ = MakeIdentity4x4();
	UpdateMatrix();
}

WorldTransform::~WorldTransform()
{
}

Matrix4x4 WorldTransform::UpdateMatrix() {

	this->matWorld_ = MakeAffineMatrix(scale_, rotation_, translation_);

	//親行列があるなら追加で掛ける
	if (parent_) {
		SetMatParent();
	}

	return this->matWorld_;

}

Matrix4x4 WorldTransform::UpdateMatrix(const Matrix4x4& matrix, bool isRotateUpdate) {

	Matrix4x4 scaleMatrix = MakeScaleMatrix(this->scale_);
	/*Matrix4x4 rotateMatrix = Multiply(MakeRotateXMatrix(this->rotation_.x),
		Multiply(MakeRotateYMatrix(this->rotation_.y), MakeRotateZMatrix(this->rotation_.z)));*/

	if (isRotateUpdate) {
		rotateMatrix_ = rotateMatrix_ * matrix;
	}
	else {
		rotateMatrix_ = matrix;
	}

	Matrix4x4 translateMatrix = MakeTranslateMatrix(this->translation_);

	this->matWorld_ = scaleMatrix * rotateMatrix_ * translateMatrix;

	//親行列があるなら追加で掛ける
	if (parent_) {
		SetMatParent();
	}

	return this->matWorld_;

}

void WorldTransform::SetMatParent() {

	if (isScaleParent_ && isRotationParent_ && isTranslationParent_) {
		this->matWorld_ = this->matWorld_ * parent_->matWorld_;
		return;
	}
	else {
		Vector3 tmpScale{ 1.0f,1.0f,1.0f }, tmpRotation{ 0.0f,0.0f,0.0f }, tmpTranslation{ 0.0f,0.0f,0.0f };

		if (isScaleParent_) {
			tmpScale = parent_->scale_;
		}

		if (isRotationParent_) {
			tmpRotation = parent_->rotation_;
		}

		if (isTranslationParent_) {
			tmpTranslation = parent_->translation_;
		}

		this->matWorld_ = this->matWorld_ * MakeAffineMatrix(tmpScale, tmpRotation, tmpTranslation);
	}

}

void WorldTransform::SetMatParent(const Matrix4x4& matrix) {

	if (isScaleParent_ && isRotationParent_ && isTranslationParent_) {
		this->matWorld_ = this->matWorld_ * parent_->matWorld_;
		return;
	}
	else {
		Vector3 tmpScale{ 1.0f,1.0f,1.0f }, tmpRotation{ 0.0f,0.0f,0.0f }, tmpTranslation{ 0.0f,0.0f,0.0f };

		if (isScaleParent_) {
			tmpScale = parent_->scale_;
		}

		if (isRotationParent_) {
			tmpRotation = parent_->rotation_;
		}

		if (isTranslationParent_) {
			tmpTranslation = parent_->translation_;
		}

		this->matWorld_ = this->matWorld_ * MakeAffineMatrix(tmpScale, tmpRotation, tmpTranslation);
	}

}
