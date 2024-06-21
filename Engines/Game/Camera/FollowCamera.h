#pragma once
#include "Camera.h"
#include "Matrix4x4.h"
#include "Vector3.h"
#include <memory>
#include "Transform.h"

class FollowCamera
{
public:
	FollowCamera();
	~FollowCamera();

	void Update();

	void SetCamera(Camera* camera) { camera_ = camera; }

	void SetTarget(Transform* target) { target_ = target; }

private:

	Camera* camera_ = nullptr;

	Transform* target_ = nullptr;

	Vector3 offset_ = { 0.0f,0.0f,-60.0f };

};

