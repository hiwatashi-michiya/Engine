#pragma once
#include "Matrix4x4.h"
#include "Quaternion.h"
#include "Vector3.h"

/// <summary>
/// スケール、回転、平行移動とワールド行列をまとめたクラス
/// </summary>
class Transform
{
public:
	
	Transform();
	~Transform();
	//行列更新
	void UpdateMatrix();
	//親セット
	void SetParent(Transform* transform) { parent_ = transform; }
	//ワールド行列
	Matrix4x4 worldMatrix_;
	//平行移動
	Vector3 translate_;
	//回転(オイラー)
	Vector3 rotate_;
	//回転(クォータニオン)
	Quaternion rotateQuaternion_;
	//スケール
	Vector3 scale_;

private:
	//親
	Transform* parent_ = nullptr;

};
