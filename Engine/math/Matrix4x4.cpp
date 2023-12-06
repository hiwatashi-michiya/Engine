#include "Matrix4x4.h"
#include "Vector3.h"
#include <cmath>
#include <cassert>

//行列の加法
Matrix4x4 Add(const Matrix4x4& m1, const Matrix4x4& m2) {

	Matrix4x4 m = {};

	for (int y = 0; y < 4; y++) {

		for (int x = 0; x < 4; x++) {

			m.m[y][x] = m1.m[y][x] + m2.m[y][x];

		}

	}

	return m;

}

//行列の減法
Matrix4x4 Subtract(const Matrix4x4& m1, const Matrix4x4& m2) {

	Matrix4x4 m = {};

	for (int y = 0; y < 4; y++) {

		for (int x = 0; x < 4; x++) {

			m.m[y][x] = m1.m[y][x] - m2.m[y][x];
		}
	}

	return m;

}

//行列の積
Matrix4x4 Multiply(const Matrix4x4& m1, const Matrix4x4& m2) {

	Matrix4x4 m = {};

	for (int y = 0; y < 4; y++) {

		for (int x = 0; x < 4; x++) {

			m.m[y][x] = m1.m[y][0] * m2.m[0][x] + m1.m[y][1] * m2.m[1][x] +
				m1.m[y][2] * m2.m[2][x] + m1.m[y][3] * m2.m[3][x];

		}

	}

	return m;

}

//逆行列
Matrix4x4 Inverse(const Matrix4x4& m) {

	//分母
	float denominator = m.m[0][0] * m.m[1][1] * m.m[2][2] * m.m[3][3] +
		m.m[0][0] * m.m[1][2] * m.m[2][3] * m.m[3][1] +
		m.m[0][0] * m.m[1][3] * m.m[2][1] * m.m[3][2] -
		m.m[0][0] * m.m[1][3] * m.m[2][2] * m.m[3][1] -
		m.m[0][0] * m.m[1][2] * m.m[2][1] * m.m[3][3] -
		m.m[0][0] * m.m[1][1] * m.m[2][3] * m.m[3][2] -
		m.m[0][1] * m.m[1][0] * m.m[2][2] * m.m[3][3] -
		m.m[0][2] * m.m[1][0] * m.m[2][3] * m.m[3][1] -
		m.m[0][3] * m.m[1][0] * m.m[2][1] * m.m[3][2] +
		m.m[0][3] * m.m[1][0] * m.m[2][2] * m.m[3][1] +
		m.m[0][2] * m.m[1][0] * m.m[2][1] * m.m[3][3] +
		m.m[0][1] * m.m[1][0] * m.m[2][3] * m.m[3][2] +
		m.m[0][1] * m.m[1][2] * m.m[2][0] * m.m[3][3] +
		m.m[0][2] * m.m[1][3] * m.m[2][0] * m.m[3][1] +
		m.m[0][3] * m.m[1][1] * m.m[2][0] * m.m[3][2] -
		m.m[0][3] * m.m[1][2] * m.m[2][0] * m.m[3][1] -
		m.m[0][2] * m.m[1][1] * m.m[2][0] * m.m[3][3] -
		m.m[0][1] * m.m[1][3] * m.m[2][0] * m.m[3][2] -
		m.m[0][1] * m.m[1][2] * m.m[2][3] * m.m[3][0] -
		m.m[0][2] * m.m[1][3] * m.m[2][1] * m.m[3][0] -
		m.m[0][3] * m.m[1][1] * m.m[2][2] * m.m[3][0] +
		m.m[0][3] * m.m[1][2] * m.m[2][1] * m.m[3][0] +
		m.m[0][2] * m.m[1][1] * m.m[2][3] * m.m[3][0] +
		m.m[0][1] * m.m[1][3] * m.m[2][2] * m.m[3][0];

	Matrix4x4 mResult = {};

	//分母0では割れない
	if (denominator == 0.0f) {
		return mResult;
	}
	

	mResult.m[0][0] = (m.m[1][1] * m.m[2][2] * m.m[3][3] + m.m[1][2] * m.m[2][3] * m.m[3][1] +
		m.m[1][3] * m.m[2][1] * m.m[3][2] - m.m[1][3] * m.m[2][2] * m.m[3][1] -
		m.m[1][2] * m.m[2][1] * m.m[3][3] - m.m[1][1] * m.m[2][3] * m.m[3][2]) /
		denominator;
	mResult.m[0][1] = (m.m[0][1] * m.m[2][2] * m.m[3][3] + m.m[0][2] * m.m[2][3] * m.m[3][1] +
		m.m[0][3] * m.m[2][1] * m.m[3][2] - m.m[0][3] * m.m[2][2] * m.m[3][1] -
		m.m[0][2] * m.m[2][1] * m.m[3][3] - m.m[0][1] * m.m[2][3] * m.m[3][2]) /
		-denominator;
	mResult.m[0][2] = (m.m[0][1] * m.m[1][2] * m.m[3][3] + m.m[0][2] * m.m[1][3] * m.m[3][1] +
		m.m[0][3] * m.m[1][1] * m.m[3][2] - m.m[0][3] * m.m[1][2] * m.m[3][1] -
		m.m[0][2] * m.m[1][1] * m.m[3][3] - m.m[0][1] * m.m[1][3] * m.m[3][2]) /
		denominator;
	mResult.m[0][3] = (m.m[0][1] * m.m[1][2] * m.m[2][3] + m.m[0][2] * m.m[1][3] * m.m[2][1] +
		m.m[0][3] * m.m[1][1] * m.m[2][2] - m.m[0][3] * m.m[1][2] * m.m[2][1] -
		m.m[0][2] * m.m[1][1] * m.m[2][3] - m.m[0][1] * m.m[1][3] * m.m[2][2]) /
		-denominator;

	mResult.m[1][0] = (m.m[1][0] * m.m[2][2] * m.m[3][3] + m.m[1][2] * m.m[2][3] * m.m[3][0] +
		m.m[1][3] * m.m[2][0] * m.m[3][2] - m.m[1][3] * m.m[2][2] * m.m[3][0] -
		m.m[1][2] * m.m[2][0] * m.m[3][3] - m.m[1][0] * m.m[2][3] * m.m[3][2]) /
		-denominator;
	mResult.m[1][1] = (m.m[0][0] * m.m[2][2] * m.m[3][3] + m.m[0][2] * m.m[2][3] * m.m[3][0] +
		m.m[0][3] * m.m[2][0] * m.m[3][2] - m.m[0][3] * m.m[2][2] * m.m[3][0] -
		m.m[0][2] * m.m[2][0] * m.m[3][3] - m.m[0][0] * m.m[2][3] * m.m[3][2]) /
		denominator;
	mResult.m[1][2] = (m.m[0][0] * m.m[1][2] * m.m[3][3] + m.m[0][2] * m.m[1][3] * m.m[3][0] +
		m.m[0][3] * m.m[1][0] * m.m[3][2] - m.m[0][3] * m.m[1][2] * m.m[3][0] -
		m.m[0][2] * m.m[1][0] * m.m[3][3] - m.m[0][0] * m.m[1][3] * m.m[3][2]) /
		-denominator;
	mResult.m[1][3] = (m.m[0][0] * m.m[1][2] * m.m[2][3] + m.m[0][2] * m.m[1][3] * m.m[2][0] +
		m.m[0][3] * m.m[1][0] * m.m[2][2] - m.m[0][3] * m.m[1][2] * m.m[2][0] -
		m.m[0][2] * m.m[1][0] * m.m[2][3] - m.m[0][0] * m.m[1][3] * m.m[2][2]) /
		denominator;

	mResult.m[2][0] = (m.m[1][0] * m.m[2][1] * m.m[3][3] + m.m[1][1] * m.m[2][3] * m.m[3][0] +
		m.m[1][3] * m.m[2][0] * m.m[3][1] - m.m[1][3] * m.m[2][1] * m.m[3][0] -
		m.m[1][1] * m.m[2][0] * m.m[3][3] - m.m[1][0] * m.m[2][3] * m.m[3][1]) /
		denominator;
	mResult.m[2][1] = (m.m[0][0] * m.m[2][1] * m.m[3][3] + m.m[0][1] * m.m[2][3] * m.m[3][0] +
		m.m[0][3] * m.m[2][0] * m.m[3][1] - m.m[0][3] * m.m[2][1] * m.m[3][0] -
		m.m[0][1] * m.m[2][0] * m.m[3][3] - m.m[0][0] * m.m[2][3] * m.m[3][1]) /
		-denominator;
	mResult.m[2][2] = (m.m[0][0] * m.m[1][1] * m.m[3][3] + m.m[0][1] * m.m[1][3] * m.m[3][0] +
		m.m[0][3] * m.m[1][0] * m.m[3][1] - m.m[0][3] * m.m[1][1] * m.m[3][0] -
		m.m[0][1] * m.m[1][0] * m.m[3][3] - m.m[0][0] * m.m[1][3] * m.m[3][1]) /
		denominator;
	mResult.m[2][3] = (m.m[0][0] * m.m[1][1] * m.m[2][3] + m.m[0][1] * m.m[1][3] * m.m[2][0] +
		m.m[0][3] * m.m[1][0] * m.m[2][1] - m.m[0][3] * m.m[1][1] * m.m[2][0] -
		m.m[0][1] * m.m[1][0] * m.m[2][3] - m.m[0][0] * m.m[1][3] * m.m[2][1]) /
		-denominator;

	mResult.m[3][0] = (m.m[1][0] * m.m[2][1] * m.m[3][2] + m.m[1][1] * m.m[2][2] * m.m[3][0] +
		m.m[1][2] * m.m[2][0] * m.m[3][1] - m.m[1][2] * m.m[2][1] * m.m[3][0] -
		m.m[1][1] * m.m[2][0] * m.m[3][2] - m.m[1][0] * m.m[2][2] * m.m[3][1]) /
		-denominator;
	mResult.m[3][1] = (m.m[0][0] * m.m[2][1] * m.m[3][2] + m.m[0][1] * m.m[2][2] * m.m[3][0] +
		m.m[0][2] * m.m[2][0] * m.m[3][1] - m.m[0][2] * m.m[2][1] * m.m[3][0] -
		m.m[0][1] * m.m[2][0] * m.m[3][2] - m.m[0][0] * m.m[2][2] * m.m[3][1]) /
		denominator;
	mResult.m[3][2] = (m.m[0][0] * m.m[1][1] * m.m[3][2] + m.m[0][1] * m.m[1][2] * m.m[3][0] +
		m.m[0][2] * m.m[1][0] * m.m[3][1] - m.m[0][2] * m.m[1][1] * m.m[3][0] -
		m.m[0][1] * m.m[1][0] * m.m[3][2] - m.m[0][0] * m.m[1][2] * m.m[3][1]) /
		-denominator;
	mResult.m[3][3] = (m.m[0][0] * m.m[1][1] * m.m[2][2] + m.m[0][1] * m.m[1][2] * m.m[2][0] +
		m.m[0][2] * m.m[1][0] * m.m[2][1] - m.m[0][2] * m.m[1][1] * m.m[2][0] -
		m.m[0][1] * m.m[1][0] * m.m[2][2] - m.m[0][0] * m.m[1][2] * m.m[2][1]) /
		denominator;

	return mResult;

}

//転置行列
Matrix4x4 Transpose(const Matrix4x4& m) {

	Matrix4x4 mResult;

	for (int y = 0; y < 4; y++) {

		for (int x = 0; x < 4; x++) {

			mResult.m[y][x] = m.m[x][y];
		}
	}

	return mResult;

}

//単位行列の作成
Matrix4x4 MakeIdentity4x4() {

	Matrix4x4 m = {};

	for (int y = 0; y < 4; y++) {

		for (int x = 0; x < 4; x++) {

			//yとxの値が等しい場合に1を入れる
			if (y == x) {

				m.m[y][x] = 1;

			}
			else {

				m.m[y][x] = 0;

			}

		}
	}

	return m;

}

//平行移動行列
Matrix4x4 MakeTranslateMatrix(const Vector3& translate) {

	Matrix4x4 m = {};

	for (int y = 0; y < 4; y++) {
		for (int x = 0; x < 4; x++) {

			if (y == x) {
				m.m[y][x] = 1.0f;
			}
			else {
				m.m[y][x] = 0.0f;
			}

		}
	}

	m.m[3][0] = translate.x;
	m.m[3][1] = translate.y;
	m.m[3][2] = translate.z;

	return m;

}

//拡大縮小行列
Matrix4x4 MakeScaleMatrix(const Vector3& scale) {

	Matrix4x4 m = {};

	for (int y = 0; y < 4; y++) {
		for (int x = 0; x < 4; x++) {

			if (y == x) {
				m.m[y][x] = 1.0f;
			}
			else {
				m.m[y][x] = 0.0f;
			}

		}
	}

	m.m[0][0] = scale.x;
	m.m[1][1] = scale.y;
	m.m[2][2] = scale.z;

	return m;

}

//X軸回転行列
Matrix4x4 MakeRotateXMatrix(float radian) {

	Matrix4x4 m{};

	for (int y = 0; y < 4; y++) {
		for (int x = 0; x < 4; x++) {
			if (y == x) {
				m.m[y][x] = 1.0f;
			}
			else {
				m.m[y][x] = 0.0f;
			}
		}
	}

	m.m[1][1] = std::cos(radian);
	m.m[1][2] = std::sin(radian);
	m.m[2][1] = -std::sin(radian);
	m.m[2][2] = std::cos(radian);

	return m;

}

//Y軸回転行列
Matrix4x4 MakeRotateYMatrix(float radian) {

	Matrix4x4 m{};

	for (int y = 0; y < 4; y++) {
		for (int x = 0; x < 4; x++) {
			if (y == x) {
				m.m[y][x] = 1.0f;
			}
			else {
				m.m[y][x] = 0.0f;
			}
		}
	}

	m.m[0][0] = std::cos(radian);
	m.m[0][2] = -std::sin(radian);
	m.m[2][0] = std::sin(radian);
	m.m[2][2] = std::cos(radian);

	return m;

}

//Z軸回転行列
Matrix4x4 MakeRotateZMatrix(float radian) {

	Matrix4x4 m{};

	for (int y = 0; y < 4; y++) {
		for (int x = 0; x < 4; x++) {
			if (y == x) {
				m.m[y][x] = 1.0f;
			}
			else {
				m.m[y][x] = 0.0f;
			}
		}
	}

	m.m[0][0] = std::cos(radian);
	m.m[0][1] = std::sin(radian);
	m.m[1][0] = -std::sin(radian);
	m.m[1][1] = std::cos(radian);

	return m;

}

//3次元アフィン変換行列
Matrix4x4 MakeAffineMatrix(const Vector3& scale, const Vector3& rotate, const Vector3& translate) {

	Matrix4x4 scaleMatrix = MakeScaleMatrix(scale);
	Matrix4x4 rotateMatrix = Multiply(MakeRotateXMatrix(rotate.x),
		Multiply(MakeRotateYMatrix(rotate.y), MakeRotateZMatrix(rotate.z)));
	Matrix4x4 translateMatrix = MakeTranslateMatrix(translate);

	Matrix4x4 m{};

	m = Multiply(Multiply(scaleMatrix, rotateMatrix), translateMatrix);

	return m;

}

//透視投影行列
Matrix4x4 MakePerspectiveFovMatrix(float fovY, float aspectRatio, float nearClip, float farClip) {

	Matrix4x4 m{};

	for (int y = 0; y < 4; y++) {
		for (int x = 0; x < 4; x++) {
			m.m[y][x] = 0.0f;
		}
	}

	m.m[0][0] = 1.0f / aspectRatio * 1.0f / tanf(fovY / 2.0f);
	m.m[1][1] = 1.0f / tanf(fovY / 2.0f);
	m.m[2][2] = farClip / (farClip - nearClip);
	m.m[2][3] = 1.0f;
	m.m[3][2] = (-nearClip * farClip) / (farClip - nearClip);

	return m;

}

//正射影行列
Matrix4x4 MakeOrthographicMatrix(float left, float top, float right, float bottom,
	float nearClip, float farClip) {

	Matrix4x4 m{};

	for (int y = 0; y < 4; y++) {
		for (int x = 0; x < 4; x++) {
			m.m[y][x] = 0.0f;
		}
	}

	m.m[0][0] = 2.0f / (right - left);
	m.m[1][1] = 2.0f / (top - bottom);
	m.m[2][2] = 1.0f / (farClip - nearClip);
	m.m[3][0] = (left + right) / (left - right);
	m.m[3][1] = (top + bottom) / (bottom - top);
	m.m[3][2] = (nearClip) / (nearClip - farClip);
	m.m[3][3] = 1.0f;

	return m;

}

//ビューポート変換行列
Matrix4x4 MakeViewportMatrix(float left, float top, float width, float height,
	float minDepth, float maxDepth) {

	Matrix4x4 m{};

	for (int y = 0; y < 4; y++) {
		for (int x = 0; x < 4; x++) {
			m.m[y][x] = 0.0f;
		}
	}

	m.m[0][0] = width / 2.0f;
	m.m[1][1] = -height / 2.0f;
	m.m[2][2] = maxDepth - minDepth;
	m.m[3][0] = left + (width / 2.0f);
	m.m[3][1] = top + (height / 2.0f);
	m.m[3][2] = minDepth;
	m.m[3][3] = 1.0f;

	return m;

}

Matrix4x4 MakeRotateAxisAngle(const Vector3& axis, float angle) {

	Matrix4x4 result{};

	result.m[0][0] = powf(axis.x, 2) * (1.0f - cosf(angle)) + cosf(angle);
	result.m[0][1] = axis.x * axis.y * (1.0f - cosf(angle)) + axis.z * sinf(angle);
	result.m[0][2] = axis.x * axis.z * (1.0f - cosf(angle)) - axis.y * sinf(angle);
	result.m[0][3] = 0.0f;

	result.m[1][0] = axis.x * axis.y * (1.0f - cosf(angle)) - axis.z * sinf(angle);
	result.m[1][1] = powf(axis.y, 2) * (1.0f - cosf(angle)) + cosf(angle);
	result.m[1][2] = axis.y * axis.z * (1.0f - cosf(angle)) + axis.x * sinf(angle);
	result.m[1][3] = 0.0f;

	result.m[2][0] = axis.x * axis.z * (1.0f - cosf(angle)) + axis.y * sinf(angle);
	result.m[2][1] = axis.y * axis.z * (1.0f - cosf(angle)) - axis.x * sinf(angle);
	result.m[2][2] = powf(axis.z, 2) * (1.0f - cosf(angle)) + cosf(angle);
	result.m[2][3] = 0.0f;

	result.m[3][0] = 0.0f;
	result.m[3][1] = 0.0f;
	result.m[3][2] = 0.0f;
	result.m[3][3] = 1.0f;

	return result;

}

//ある方向をある方向へ向ける回転行列
Matrix4x4 DirectionToDirection(const Vector3& from, const Vector3& to) {

	//回転軸
	Vector3 n;
	
	//cosを求める
	float cosTheta = Dot(from, to);

	//逆向きのベクトルだった場合、垂直なベクトルを一つ選ぶ
	if (cosTheta <= -1.0f) {

		if (from.x != 0.0f || from.y != 0.0f) {

			n = { from.y, -from.x,0.0f };
			n = Normalize(n);
		}
		else if (from.x != 0.0f || from.z != 0.0f) {
			
			n = { 0.0f, -from.z, from.x };
			n = Normalize(n);

		}

	}
	else {
		n = Normalize(Cross(from, to));
	}

	//sinを求める
	float sinTheta = Length(Cross(from, to));

	Matrix4x4 result = MakeIdentity4x4();

	//任意軸の回転行列を求めた値から生成
	result.m[0][0] = powf(n.x, 2) * (1.0f - cosTheta) + cosTheta;
	result.m[0][1] = n.x * n.y * (1.0f - cosTheta) + n.z * sinTheta;
	result.m[0][2] = n.x * n.z * (1.0f - cosTheta) - n.y * sinTheta;
	result.m[0][3] = 0.0f;

	result.m[1][0] = n.x * n.y * (1.0f - cosTheta) - n.z * sinTheta;
	result.m[1][1] = powf(n.y, 2) * (1.0f - cosTheta) + cosTheta;
	result.m[1][2] = n.y * n.z * (1.0f - cosTheta) + n.x * sinTheta;
	result.m[1][3] = 0.0f;

	result.m[2][0] = n.x * n.z * (1.0f - cosTheta) + n.y * sinTheta;
	result.m[2][1] = n.y * n.z * (1.0f - cosTheta) - n.x * sinTheta;
	result.m[2][2] = powf(n.z, 2) * (1.0f - cosTheta) + cosTheta;
	result.m[2][3] = 0.0f;

	result.m[3][0] = 0.0f;
	result.m[3][1] = 0.0f;
	result.m[3][2] = 0.0f;
	result.m[3][3] = 1.0f;

	return result;

}

// 二項演算子
Matrix4x4 operator+(const Matrix4x4& m1, const Matrix4x4& m2) { return Add(m1, m2); }
Matrix4x4 operator-(const Matrix4x4& m1, const Matrix4x4& m2) { return Subtract(m1, m2); }
Matrix4x4 operator*(const Matrix4x4& m1, const Matrix4x4& m2) { return Multiply(m1, m2); }
