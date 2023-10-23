#pragma once

/// <summary>
/// 3次元ベクトル
/// </summary>
class Vector3 {
public:

	float x;
	float y;
	float z;

	Vector3& operator*=(float s) { x *= s; y *= s; z *= s; return *this; }
	Vector3& operator-=(const Vector3& v) { x -= v.x;  y -= v.y; z -= v.z; return *this; }
	Vector3& operator+=(const Vector3& v) { x += v.x;  y += v.y; z += v.z; return *this; }
	Vector3& operator/=(float s) { x /= s; y /= s; z /= s; return *this; }

};

struct Transform {
	Vector3 scale;
	Vector3 rotate;
	Vector3 translate;
};

class Matrix4x4;

Vector3 Add(const Vector3& v1, const Vector3& v2);

Vector3 Subtract(const Vector3& v1, const Vector3& v2);

Vector3 Multiply(float scalar, const Vector3& v1);

float Dot(const Vector3& v1, const Vector3& v2);

float Length(const Vector3& v);

Vector3 Normalize(const Vector3& v);

//座標変換
Vector3 CoordTransform(const Vector3& vector, const Matrix4x4& matrix);

//クロス積
Vector3 Cross(const Vector3& v1, const Vector3& v2);

//二項演算子
Vector3 operator+(const Vector3& v1, const Vector3& v2);
Vector3 operator-(const Vector3& v1, const Vector3& v2);
Vector3 operator*(float s, const Vector3& v);
Vector3 operator*(const Vector3& v, float s);
Vector3 operator/(const Vector3& v, float s);

//単項演算子
Vector3 operator-(const Vector3& v);
Vector3 operator+(const Vector3& v);
