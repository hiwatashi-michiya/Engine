#pragma once
#include "Camera.h"
#include "Matrix4x4.h"
#include "Vector3.h"
#include <memory>
#include "Transform.h"
#include "Game/Variables/CommonVariables.h"

class FollowCamera
{
public:
	FollowCamera();
	~FollowCamera();

	void Initialize();

	void Update();

	void SetCamera(Camera* camera) { camera_ = camera; }

	void SetTarget(Transform* target) { target_ = target; }

	static const CommonVariables::CameraType& GetCameraType() { return cameraType_; }

	bool GetIsSwitching() const { return isSwitching_; }

	bool GetIsSwitched() const { return isSwitched_; }

	void SetCanSwitch(bool flag) { canSwitch_ = flag; }

private:

	Camera* camera_ = nullptr;

	Transform* target_ = nullptr;

	Vector3 offset_ = { 0.0f,0.0f,-100.0f };
	//カメラ視点切り替えフラグ
	bool isSwitching_ = false;
	//切り替え終わった瞬間のフラグ
	bool isSwitched_ = false;
	//カメラの切り替えが可能かどうか
	bool canSwitch_ = true;
	//イージングの時間カウント
	int32_t easingCount_ = 0;
	//最大イージング時間
	int32_t maxEasingTime_ = 30;
	//カメラのアングル
	Vector3 sideAngle_{ 0.0f,0.0f,0.0f };
	Vector3 aboveAngle_{ 1.57f,0.0f,0.0f };


	static CommonVariables::CameraType cameraType_;

};

