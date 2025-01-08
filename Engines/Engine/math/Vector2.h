#pragma once

/// <summary>
/// 2次元ベクトル
/// </summary>
struct Vector2 {
	float x;
	float y;
};

//線形補間
Vector2 Lerp(const Vector2& v1, const Vector2& v2, float t);
