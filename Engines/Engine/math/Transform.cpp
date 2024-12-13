#include "Transform.h"

Transform::Transform()
{
	//値をデフォルトに設定
	scale_ = Vector3::Identity();
	rotate_ = { 0.0f,0.0f,0.0f };
	rotateQuaternion_ = rotateQuaternion_.IdentityQuaternion();
	translate_ = { 0.0f,0.0f,0.0f };

	worldMatrix_ = MakeAffineMatrix(scale_, rotate_, translate_);

}

Transform::~Transform()
{
}

void Transform::UpdateMatrix() {
	//クォータニオン正規化
	rotateQuaternion_.Normalize();
	//行列作成、更新
	worldMatrix_ = MakeAffineMatrix(scale_, rotateQuaternion_, translate_);
	//親がいたら親行列も掛ける
	if (parent_) {
		worldMatrix_ = worldMatrix_ * parent_->worldMatrix_;
	}

}
