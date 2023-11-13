#pragma once

/// <summary>
/// 4x4行列
/// </summary>
struct Matrix4x4 {

	float m[4][4];

	////初期化
	//Matrix4x4() {
	//	
	//	for (int y = 0; y < 4; y++) {

	//		for (int x = 0; x < 4; x++) {

	//			//yとxの値が等しい場合に1を入れる
	//			if (y == x) {

	//				m[y][x] = 1;

	//			}
	//			else {

	//				m[y][x] = 0;

	//			}

	//		}
	//	}

	//}

	Matrix4x4& operator+=(const Matrix4x4& matrix) {
		

		for (int y = 0; y < 4; y++) {

			for (int x = 0; x < 4; x++) {

				m[y][x] = m[y][x] + matrix.m[y][x];

			}

		}

		return *this;
	}

	Matrix4x4& operator-=(const Matrix4x4& matrix) {
		
		for (int y = 0; y < 4; y++) {

			for (int x = 0; x < 4; x++) {

				m[y][x] = m[y][x] - matrix.m[y][x];
			}
		}

		return *this;
	}

};

struct TransformationMatrix {
	Matrix4x4 WVP;
	Matrix4x4 World;
};

struct Vector3;

//行列の加法
Matrix4x4 Add(const Matrix4x4& m1, const Matrix4x4& m2);

//行列の減法
Matrix4x4 Subtract(const Matrix4x4& m1, const Matrix4x4& m2);

//行列の積
Matrix4x4 Multiply(const Matrix4x4& m1, const Matrix4x4& m2);

//逆行列
Matrix4x4 Inverse(const Matrix4x4& m);

//転置行列
Matrix4x4 Transpose(const Matrix4x4& m);

//単位行列の作成
Matrix4x4 MakeIdentity4x4();

//平行移動行列
Matrix4x4 MakeTranslateMatrix(const Vector3& translate);

//拡大縮小行列
Matrix4x4 MakeScaleMatrix(const Vector3& scale);

//X軸回転行列
Matrix4x4 MakeRotateXMatrix(float radian);

//Y軸回転行列
Matrix4x4 MakeRotateYMatrix(float radian);

//Z軸回転行列
Matrix4x4 MakeRotateZMatrix(float radian);

//3次元アフィン変換行列
Matrix4x4 MakeAffineMatrix(const Vector3& scale, const Vector3& rotate, const Vector3& translate);

//透視投影行列
Matrix4x4 MakePerspectiveFovMatrix(float fovY, float aspectRatio, float nearClip, float farClip);

//正射影行列
Matrix4x4 MakeOrthographicMatrix(float left, float top, float right, float bottom,
	float nearClip, float farClip);

//ビューポート変換行列
Matrix4x4 MakeViewportMatrix(float left, float top, float width, float height,
	float minDepth, float maxDepth);

// 二項演算子
Matrix4x4 operator+(const Matrix4x4& m1, const Matrix4x4& m2);
Matrix4x4 operator-(const Matrix4x4& m1, const Matrix4x4& m2);
Matrix4x4 operator*(const Matrix4x4& m1, const Matrix4x4& m2);
