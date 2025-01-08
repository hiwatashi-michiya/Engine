#pragma once

/// <summary>
/// 4次元ベクトル
/// </summary>
class Vector4 {
public:

	float x;
	float y;
	float z;
	float w;

	Vector4& operator*=(float s) { x *= s; y *= s; z *= s; w *= s; return *this; }
	Vector4& operator-=(const Vector4& v) { x -= v.x;  y -= v.y; z -= v.z; w -= v.w; return *this; }
	Vector4& operator+=(const Vector4& v) { x += v.x;  y += v.y; z += v.z; w += v.w; return *this; }
	Vector4& operator/=(float s) { x /= s; y /= s; z /= s; w /= s; return *this; }

};

//加算
Vector4 Add(const Vector4& v1, const Vector4& v2);
//減算
Vector4 Subtract(const Vector4& v1, const Vector4& v2);
//乗算
Vector4 Multiply(float scalar, const Vector4& v1);

// 線形補間
Vector4 Lerp(const Vector4& v1, const Vector4& v2, float t);

//二項演算子
Vector4 operator+(const Vector4& v1, const Vector4& v2);
Vector4 operator-(const Vector4& v1, const Vector4& v2);
Vector4 operator*(float s, const Vector4& v);
Vector4 operator*(const Vector4& v, float s);
Vector4 operator/(const Vector4& v, float s);

//単項演算子
Vector4 operator-(const Vector4& v);
Vector4 operator+(const Vector4& v);
