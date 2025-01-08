#include "Vector4.h"
#include <algorithm>


Vector4 Add(const Vector4& v1, const Vector4& v2)
{
	return Vector4{ v1.x + v2.x, v1.y + v2.y, v1.z + v2.z, v1.w + v2.w };
}

Vector4 Subtract(const Vector4& v1, const Vector4& v2)
{
	return Vector4{ v1.x - v2.x, v1.y - v2.y, v1.z - v2.z, v1.w - v2.w };
}

Vector4 Multiply(float scalar, const Vector4& v1)
{
	
	Vector4 v;

	v.x = scalar * v1.x;
	v.y = scalar * v1.y;
	v.z = scalar * v1.z;
	v.w = scalar * v1.w;

	return v;

}

Vector4 Lerp(const Vector4& v1, const Vector4& v2, float t)
{

	t = std::clamp(t, 0.0f, 1.0f);

	Vector4 p = Vector4(
		v1.x + t * (v2.x - v1.x),
		v1.y + t * (v2.y - v1.y),
		v1.z + t * (v2.z - v1.z),
		v1.w + t * (v2.w - v1.w)
	);

	return p;

}

Vector4 operator+(const Vector4& v1, const Vector4& v2) { return Add(v1, v2); }
Vector4 operator-(const Vector4& v1, const Vector4& v2) { return Subtract(v1, v2); }

Vector4 operator*(float s, const Vector4& v)
{
	return Multiply(s, v);
}

Vector4 operator*(const Vector4& v, float s)
{
	return s * v;
}

Vector4 operator/(const Vector4& v, float s)
{
	return Multiply(1.0f / s, v);
}

Vector4 operator-(const Vector4& v) { return { -v.x, -v.y, -v.z, -v.w }; }
Vector4 operator+(const Vector4& v) { return v; }
