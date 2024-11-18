#pragma once

struct Vector4 {
	float x;
	float y;
	float z;
	float w;
};

// 線形補間
Vector4 Lerp(const Vector4& v1, const Vector4& v2, float t);
