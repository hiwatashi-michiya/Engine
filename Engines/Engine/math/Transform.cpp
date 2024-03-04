#include "Transform.h"

Transform::Transform()
{

	scale_ = Vector3::Identity();
	rotate_ = { 0.0f,0.0f,0.0f };
	translate_ = { 0.0f,0.0f,0.0f };

	worldMatrix_ = MakeAffineMatrix(scale_, rotate_, translate_);

}

Transform::~Transform()
{
}

void Transform::UpdateMatrix() {

	worldMatrix_ = MakeAffineMatrix(scale_, rotate_, translate_);

	if (parent_) {
		worldMatrix_ = worldMatrix_ * parent_->worldMatrix_;
	}

}
