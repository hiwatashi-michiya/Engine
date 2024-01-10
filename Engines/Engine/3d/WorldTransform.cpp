#include "WorldTransform.h"

WorldTransform::WorldTransform()
{
	matWorld_ = MakeAffineMatrix(scale_, rotation_, translation_);
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

void WorldTransform::SetMatParent() {

	if (isScale_ && isRotation_ && isTranslation_) {
		this->matWorld_ = MakeAffineMatrix(scale_, rotation_, translation_);
	}
	else {
		Vector3 tmpScale{ 1.0f,1.0f,1.0f }, tmpRotation{ 0.0f,0.0f,0.0f }, tmpTranslation{ 0.0f,0.0f,0.0f };

		if (isScale_) {
			tmpScale = this->scale_;
		}

		if (isRotation_) {
			tmpRotation = this->rotation_;
		}

		if (isTranslation_) {
			tmpTranslation = this->translation_;
		}

		this->matWorld_ = MakeAffineMatrix(tmpScale, tmpRotation, tmpTranslation);

	}

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
