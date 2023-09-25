#include "Matrix4x4.h"
#include "Vector3.h"
#include <cmath>

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

	//分母0では割れない
	assert(denominator != 0.0f);

	Matrix4x4 mResult = {};

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
