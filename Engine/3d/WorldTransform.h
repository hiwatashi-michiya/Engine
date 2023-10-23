#pragma once
#include "Engine/math/Matrix4x4.h"
#include "Engine/math/Vector3.h"

class WorldTransform
{
public:
	WorldTransform();
	~WorldTransform();

	//ローカルスケール
	Vector3 scale_ = { 1.0f,1.0f,1.0f };

	//ローカル回転角
	Vector3 rotation_ = { 0.0f,0.0f,0.0f };

	//ローカル座標
	Vector3 translation_ = { 0.0f,0.0f,0.0f };

	//親のワールドトランスフォームのポインタ
	WorldTransform* parent_ = nullptr;

	//ワールド行列
	Matrix4x4 matWorld_;

	Matrix4x4 UpdateMatrix();

private:



};

