#pragma once

/// <summary>
/// 4次元ベクトル
/// </summary>
struct Vector4 {
	float x;
	float y;
	float z;
	float w;
};

//加算
Vector4 Add(const Vector4& v1, const Vector4& v2);
//減算
Vector4 Subtract(const Vector4& v1, const Vector4& v2);

// 線形補間
Vector4 Lerp(const Vector4& v1, const Vector4& v2, float t);

//二項演算子
Vector4 operator+(const Vector4& v1, const Vector4& v2);
Vector4 operator-(const Vector4& v1, const Vector4& v2);

//単項演算子
Vector4 operator-(const Vector4& v);
Vector4 operator+(const Vector4& v);
