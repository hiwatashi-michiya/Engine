#pragma once
#include "Matrix4x4.h"
#include "Vector3.h"
#include "Quaternion.h"

//GPUで利用するカメラ座標
struct CameraForGPU
{
	Vector3 worldPosition;
};

class Camera
{
public:

	void Initialize();

	void Update();

	//ローカルスケール
	Vector3 scale_ = { 1.0f,1.0f,1.0f };

	//ローカル回転角
	Vector3 rotation_ = { 0.0f,0.0f,0.0f };

	//ローカル座標
	Vector3 position_ = { 0.0f,0.0f,0.0f };

	//回転行列
	Matrix4x4 matRotate_ = MakeIdentity4x4();

	//ワールド行列(カメラ行列)
	Matrix4x4 matWorld_;

	//ビュー行列
	Matrix4x4 matView_;
	//プロジェクション行列
	Matrix4x4 matProjection_;
	//ビュープロジェクション行列
	Matrix4x4 matViewProjection_;

	Vector3 GetWorldPosition() {

		Vector3 pos;

		pos.x = matWorld_.m[3][0];
		pos.y = matWorld_.m[3][1];
		pos.z = matWorld_.m[3][2];

		return pos;

	}


private:

	//クォータニオン回転
	Quaternion rotateQuaternion_ = { 0.0f,0.0f,0.0f,1.0f };

};

