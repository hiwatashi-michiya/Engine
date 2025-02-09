#pragma once
#include "Camera.h"
#include "Matrix4x4.h"
#include "Vector3.h"
#include <memory>
#include "Transform.h"

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

private:
	//カメラ
	Camera* camera_ = nullptr;
	//ターゲット
	Transform* target_ = nullptr;
	//オフセット
	Vector3 offset_ = { 0.0f,0.0f,-100.0f };
	//初期の回転
	Vector3 startAngle_ = { 1.0f,0.0f,0.0f };

};

