#pragma once
#include "Camera.h"
#include <memory>
#include "Vector3.h"
#include "Input.h"

class DebugCamera
{
public:
	DebugCamera();
	~DebugCamera();

	Camera* GetCamera() { return camera_.get(); }

	void Initialize();

	void Update();

	void Debug();

	void SetPosition(const Vector3& position) { camera_->position_ = position; }

	void SetRotate(const Vector3& rotate) { camera_->rotation_ = rotate; }

private:

	Input* input_ = nullptr;

	std::shared_ptr<Camera> camera_;

};


