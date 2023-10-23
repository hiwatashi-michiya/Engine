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

	if (parent_) {
		this->matWorld_ *= parent_->matWorld_;
	}

	return this->matWorld_;

}
