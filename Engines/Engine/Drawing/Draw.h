#pragma once
#include "Vector3.h"
#include "Vector4.h"
#include "Matrix4x4.h"
#include <stdint.h>

/// <summary>
/// 球体
/// </summary>
struct Sphere
{
	Vector3 center;
	float radius;
};
/// <summary>
/// 平面
/// </summary>
struct Plane {
	Vector3 normal; //法線
	float distance; //距離
};

//球体ライン描画
void DrawSphere(const Sphere& sphere, const Matrix4x4& viewProjectionMatrix,
	const Matrix4x4& viewportMatrix);
//グリッドライン描画
void DrawGrid(const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix);
//平面ライン描画
void DrawPlane(const Plane& plane, const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix);
//三角形ライン描画
void DrawTriangle(const Triangle& triangle, const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix);
//AABBライン描画
void DrawAABB(const AABB& aabb, const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix);
