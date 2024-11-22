#pragma once
#include "Vector3.h"
#include "Vector4.h"
#include "Matrix4x4.h"
#include <stdint.h>

struct Sphere
{
	Vector3 center;
	float radius;
};

struct Plane {
	Vector3 normal; //法線
	float distance; //距離
};

void DrawSphere(const Sphere& sphere, const Matrix4x4& viewProjectionMatrix,
	const Matrix4x4& viewportMatrix);

void DrawGrid(const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix);

void DrawPlane(const Plane& plane, const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix);

void DrawTriangle(const Triangle& triangle, const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix);

void DrawAABB(const AABB& aabb, const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix);
