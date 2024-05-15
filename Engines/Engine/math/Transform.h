#pragma once
#include "Matrix4x4.h"
#include "Quaternion.h"
#include "Vector3.h"

class Transform
{
public:
	
	Transform();
	~Transform();

	void UpdateMatrix();

	void SetParent(Transform* transform) { parent_ = transform; }

	Matrix4x4 worldMatrix_;

	Vector3 translate_;

	Vector3 rotate_;

	Quaternion rotateQuaternion_;

	Vector3 scale_;

private:

	Transform* parent_;

};
