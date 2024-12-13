#pragma once
#include "Camera.h"
#include "Matrix4x4.h"
#include "Vector3.h"
#include <memory>
#include "Transform.h"
#include "Game/Variables/CommonVariables.h"

/// <summary>
/// ターゲットに追従するカメラを動かすクラス
/// </summary>
class FollowCamera
{
public:
	FollowCamera();
	~FollowCamera();
	//初期化
	void Initialize();
	//更新
	void Update();
	//カメラセット
	void SetCamera(Camera* camera) { camera_ = camera; }
	//ターゲットセット
	void SetTarget(Transform* target) { target_ = target; }
	//カメラの現在の向き取得
	static const CommonVariables::CameraType& GetCameraType() { return cameraType_; }
	//カメラ切り替えfフラグ取得
	bool GetIsSwitching() const { return isSwitching_; }
	//カメラを切り替え終えたフラグ取得
	bool GetIsSwitched() const { return isSwitched_; }
	//切り替え可能かどうかのフラグセット
	void SetCanSwitch(bool flag) { canSwitch_ = flag; }

private:
	//カメラ
	Camera* camera_ = nullptr;
	//ターゲット
	Transform* target_ = nullptr;
	//オフセット
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

