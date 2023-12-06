#pragma once
#include "Vector3.h"

/// <summary>
/// 四元数。クォータニオン
/// </summary>
class Quaternion
{
public:
	Quaternion();
	Quaternion(float x, float y, float z, float w);

	float x;
	float y;
	float z;
	float w;

	/// <summary>
	/// 自身と引数のクォータニオンの積
	/// </summary>
	/// <param name="hs">自身に掛けるクォータニオン</param>
	/// <returns></returns>
	Quaternion Multiply(const Quaternion& hs);

	/// <summary>
	/// 単位クォータニオンを返す
	/// </summary>
	/// <returns></returns>
	Quaternion IdentityQuaternion();

	/// <summary>
	/// 共役クォータニオンを返す
	/// </summary>
	/// <returns></returns>
	Quaternion ConjuGate();

	/// <summary>
	/// クォータニオンのノルムを返す
	/// </summary>
	/// <returns></returns>
	float Norm();

	/// <summary>
	/// 正規化したクォータニオンを返す
	/// </summary>
	/// <returns></returns>
	Quaternion Normalize();

	/// <summary>
	/// 逆クォータニオンを返す
	/// </summary>
	/// <returns></returns>
	Quaternion Inverse();

	/// <summary>
	/// 任意軸回転
	/// </summary>
	/// <param name="axis">任意軸</param>
	/// <param name="angle">回転量</param>
	/// <returns></returns>
	Quaternion MakeRotateAxisAngleQuaternion(const Vector3& axis, float angle);

	Quaternion operator*=(const Quaternion& qr) { return this->Multiply(qr); }

};

/// <summary>
/// クォータニオンの積
/// </summary>
/// <param name="lhs"></param>
/// <param name="rhs"></param>
/// <returns></returns>
Quaternion Multiply(const Quaternion& lhs, const Quaternion& rhs);

/// <summary>
/// 共役クォータニオンを返す
/// </summary>
/// <param name="quaternion"></param>
/// <returns></returns>
Quaternion ConjuGate(const Quaternion& quaternion);

/// <summary>
/// クォータニオンのノルムを返す
/// </summary>
/// <param name="quaternion"></param>
/// <returns></returns>
float Norm(const Quaternion& quaternion);

/// <summary>
/// 正規化したクォータニオンを返す
/// </summary>
/// <param name="quaternion"></param>
/// <returns></returns>
Quaternion Normalize(const Quaternion& quaternion);

/// <summary>
/// 逆クォータニオンを返す
/// </summary>
/// <param name="quaternion"></param>
/// <returns></returns>
Quaternion Inverse(const Quaternion& quaternion);

/// <summary>
/// 任意軸回転
/// </summary>
/// <param name="axis">任意軸</param>
/// <param name="angle">回転量</param>
/// <returns></returns>
Quaternion MakeRotateAxisAngleQuaternion(const Vector3& axis, float angle);

Quaternion operator*(const Quaternion& qr1, const Quaternion& qr2);
