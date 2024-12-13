#pragma once
#include "Camera.h"
#include <memory>
#include "Vector3.h"
#include "Input.h"

/// <summary>
/// マウスで操作可能なデバッグ用のカメラ
/// </summary>
class DebugCamera
{
public:
	DebugCamera();
	~DebugCamera();

	//カメラ取得
	Camera* GetCamera() { return camera_.get(); }
	//初期化
	void Initialize();
	//更新
	void Update();
	//デバッグ
	void Debug();
	//位置セット
	void SetPosition(const Vector3& position) { camera_->position_ = position; }
	//回転セット
	void SetRotate(const Vector3& rotate) { camera_->rotation_ = rotate; }

private:

	Input* input_ = nullptr;

	std::shared_ptr<Camera> camera_;

	//初期座標
	Vector3 startPosition_ = { 0.0f,65.0f, -60.0f };
	
	//初期回転
	float startRotate_ = 0.9f;

	//回転感度
	float rotateSensi_ = 0.003f;
	//移動感度
	float moveSensi_ = 0.10f;

};


