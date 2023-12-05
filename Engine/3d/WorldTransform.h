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

	//スケールを引き継ぐか
	bool isScaleParent_ = true;

	//回転を引き継ぐか
	bool isRotationParent_ = true;

	//平行移動を引き継ぐか
	bool isTranslationParent_ = true;

	//自身のスケールを維持するか
	bool isScale_ = true;

	//自身の回転を維持するか
	bool isRotation_ = true;

	//自身の平行移動を維持するか
	bool isTranslation_ = true;

	//ワールド行列
	Matrix4x4 matWorld_;

	Matrix4x4 UpdateMatrix();

	const Vector3& GetWorldPosition() {

		Vector3 pos;

		pos.x = matWorld_.m[3][0];
		pos.y = matWorld_.m[3][1];
		pos.z = matWorld_.m[3][2];

		return pos;

	}

private:

	void SetMatParent();

};

