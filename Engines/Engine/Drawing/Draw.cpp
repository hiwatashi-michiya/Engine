#include "Draw.h"
#include <cmath>
#include <numbers>

void DrawSphere(const Sphere& sphere, const Matrix4x4& viewProjectionMatrix,
	const Matrix4x4& viewportMatrix) {
	const uint32_t kSubdivision = 16; //分割数
	const float  kLonEvery = float(2 * std::numbers::pi / kSubdivision); //経度分割1つ分の角度
	const float kLatEvery = float(2 * std::numbers::pi / kSubdivision); //緯度分割1つ分の角度
	//緯度の方向に分割　-π/2 ～ π/2
	for (uint32_t latIndex = 0; latIndex < kSubdivision; ++latIndex) {
		float lat = -(float(std::numbers::pi) / 2.0f) + kLatEvery * latIndex; //現在の緯度
		//経度の方向に分割　-π/2 ～ π/2
		for (uint32_t lonIndex = 0; lonIndex < kSubdivision; ++lonIndex) {
			float lon = lonIndex * kLonEvery;//現在の経度
			float thetaD = float(std::numbers::pi / kSubdivision);
			float phiD = float(2 * std::numbers::pi / kSubdivision);
			//world座標系でのa,b,cを求める
			Vector3 a, b, c;
			a = { sphere.radius * cosf(lat) * cosf(lon) + sphere.center.x,
				sphere.radius * sinf(lat) + sphere.center.y,
				sphere.radius * cosf(lat) * sinf(lon) + sphere.center.z };

			b = { sphere.radius * cosf(lat + thetaD) * cosf(lon) + sphere.center.x,
				sphere.radius * sinf(lat + thetaD) + sphere.center.y,
			sphere.radius * cosf(lat + thetaD) * sinf(lon) + sphere.center.z };

			c = { sphere.radius * cosf(lat) * cosf(lon + phiD) + sphere.center.x,
			sphere.radius * sinf(lat) + sphere.center.y,
			sphere.radius * cosf(lat) * sinf(lon + phiD) + sphere.center.z };

			//スクリーン座標に変換
			Vector3 ndcVertex = CoordTransform(a, viewProjectionMatrix);
			a = CoordTransform(ndcVertex, viewportMatrix);
			ndcVertex = CoordTransform(b, viewProjectionMatrix);
			b = CoordTransform(ndcVertex, viewportMatrix);
			ndcVertex = CoordTransform(c, viewProjectionMatrix);
			c = CoordTransform(ndcVertex, viewportMatrix);

			//ab,acで線を引く
			/*Novice::DrawLine(int(a.x), int(a.y), int(b.x), int(b.y), color);
			Novice::DrawLine(int(a.x), int(a.y), int(c.x), int(c.y), color);*/

		}
	}

}

void DrawGrid(const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix) {

	const float kGridHalfWidth = 2.0f; //Gridの半分の幅
	const uint32_t kSubdivision = 10; //分割数
	const float kGridEvery = (kGridHalfWidth * 2.0f) / float(kSubdivision); //1つ分の長さ

	//奥から手前への線を順々に引いていく
	for (uint32_t x = 0; x <= kSubdivision; ++x) {

		//座標を求める
		Vector3 v1 = { kGridEvery * x - kGridHalfWidth, 0.0f,kGridHalfWidth };
		Vector3 v2 = { kGridEvery * x - kGridHalfWidth,0.0f,-kGridHalfWidth };

		//スクリーン座標に変換
		Vector3 ndcVertex = CoordTransform(v1, viewProjectionMatrix);
		v1 = CoordTransform(ndcVertex, viewportMatrix);
		ndcVertex = CoordTransform(v2, viewProjectionMatrix);
		v2 = CoordTransform(ndcVertex, viewportMatrix);

		//変換後の座標を使って表示
		/*Novice::DrawLine(int(v1.x), int(v1.y), int(v2.x), int(v2.y), 0xAAAAAAFF);*/

	}

	//左右
	for (uint32_t z = 0; z <= kSubdivision; ++z) {

		//座標を求める
		Vector3 v1 = { kGridHalfWidth, 0.0f,kGridEvery * z - kGridHalfWidth };
		Vector3 v2 = { -kGridHalfWidth,0.0f,kGridEvery * z - kGridHalfWidth };

		//スクリーン座標に変換
		Vector3 ndcVertex = CoordTransform(v1, viewProjectionMatrix);
		v1 = CoordTransform(ndcVertex, viewportMatrix);
		ndcVertex = CoordTransform(v2, viewProjectionMatrix);
		v2 = CoordTransform(ndcVertex, viewportMatrix);

		//変換後の座標を使って表示
		/*Novice::DrawLine(int(v1.x), int(v1.y), int(v2.x), int(v2.y), 0xAAAAAAFF);*/

	}

}

void DrawPlane(const Plane& plane, const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix) {
	//中心点を決める
	Vector3 center = Multiply(plane.distance, plane.normal);
	//描画に使う四つのベクトルを定義
	Vector3 perpendiculars[4];
	//法線と垂直なベクトルを一つ求める
	perpendiculars[0] = Normalize(Perpendicular(plane.normal));
	//上で求めた垂直ベクトルの逆ベクトルを求める
	perpendiculars[1] = { -perpendiculars[0].x, -perpendiculars[0].y, -perpendiculars[0].z };
	//垂直ベクトルと法線のクロス積を求める
	perpendiculars[2] = Cross(plane.normal, perpendiculars[0]);
	//上で求めたクロス積の逆ベクトルを求める
	perpendiculars[3] = { -perpendiculars[2].x, -perpendiculars[2].y, -perpendiculars[2].z };
	//上記の四つのベクトルを中心点にそれぞれ定数倍して足すと四頂点が出来上がる
	Vector3 points[4];
	for (int32_t index = 0; index < 4; ++index) {
		Vector3 extend = Multiply(2.0f, perpendiculars[index]);
		Vector3 point = Add(center, extend);
		points[index] = CoordTransform(CoordTransform(point, viewProjectionMatrix), viewportMatrix);
	}

	//pointsを結んで描画
	/*Novice::DrawLine(int(points[0].x), int(points[0].y), int(points[3].x), int(points[3].y), color);
	Novice::DrawLine(int(points[1].x), int(points[1].y), int(points[3].x), int(points[3].y), color);
	Novice::DrawLine(int(points[2].x), int(points[2].y), int(points[1].x), int(points[1].y), color);
	Novice::DrawLine(int(points[2].x), int(points[2].y), int(points[0].x), int(points[0].y), color);*/

}


void DrawTriangle(const Triangle& triangle, const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix) {

	Vector3 point[3]{};

	for (int i = 0; i < 3; i++) {
		point[i] = CoordTransform(CoordTransform(triangle.vertices[i], viewProjectionMatrix), viewportMatrix);
	}

	/*Novice::DrawTriangle(int(point[0].x), int(point[0].y),
		int(point[1].x), int(point[1].y),
		int(point[2].x), int(point[2].y),
		color, kFillModeWireFrame);*/

}

void DrawAABB(const AABB& aabb, const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix) {

	Vector3 point[8]{};

	//それぞれの位置を計算

	//左下前
	point[0] = { aabb.min.x,aabb.min.y, aabb.min.z };
	//右下前
	point[1] = { aabb.max.x,aabb.min.y, aabb.min.z };
	//左上前
	point[2] = { aabb.min.x,aabb.max.y, aabb.min.z };
	//右上前
	point[3] = { aabb.max.x,aabb.max.y, aabb.min.z };
	//左下奥
	point[4] = { aabb.min.x,aabb.min.y, aabb.max.z };
	//右下奥
	point[5] = { aabb.max.x,aabb.min.y, aabb.max.z };
	//左上奥
	point[6] = { aabb.min.x,aabb.max.y, aabb.max.z };
	//右上奥
	point[7] = { aabb.max.x,aabb.max.y, aabb.max.z };

	//左下前
	point[0] = CoordTransform(CoordTransform(point[0], viewProjectionMatrix), viewportMatrix);
	//右下前
	point[1] = CoordTransform(CoordTransform(point[1], viewProjectionMatrix), viewportMatrix);
	//左上前
	point[2] = CoordTransform(CoordTransform(point[2], viewProjectionMatrix), viewportMatrix);
	//右上前
	point[3] = CoordTransform(CoordTransform(point[3], viewProjectionMatrix), viewportMatrix);
	//左下奥
	point[4] = CoordTransform(CoordTransform(point[4], viewProjectionMatrix), viewportMatrix);
	//右下奥
	point[5] = CoordTransform(CoordTransform(point[5], viewProjectionMatrix), viewportMatrix);
	//左上奥
	point[6] = CoordTransform(CoordTransform(point[6], viewProjectionMatrix), viewportMatrix);
	//右上奥
	point[7] = CoordTransform(CoordTransform(point[7], viewProjectionMatrix), viewportMatrix);

	//八頂点を使って描画
	/*Novice::DrawLine(int(point[0].x), int(point[0].y), int(point[1].x), int(point[1].y), color);
	Novice::DrawLine(int(point[0].x), int(point[0].y), int(point[2].x), int(point[2].y), color);
	Novice::DrawLine(int(point[1].x), int(point[1].y), int(point[3].x), int(point[3].y), color);
	Novice::DrawLine(int(point[2].x), int(point[2].y), int(point[3].x), int(point[3].y), color);

	Novice::DrawLine(int(point[0].x), int(point[0].y), int(point[4].x), int(point[4].y), color);
	Novice::DrawLine(int(point[1].x), int(point[1].y), int(point[5].x), int(point[5].y), color);
	Novice::DrawLine(int(point[2].x), int(point[2].y), int(point[6].x), int(point[6].y), color);
	Novice::DrawLine(int(point[3].x), int(point[3].y), int(point[7].x), int(point[7].y), color);

	Novice::DrawLine(int(point[4].x), int(point[4].y), int(point[5].x), int(point[5].y), color);
	Novice::DrawLine(int(point[4].x), int(point[4].y), int(point[6].x), int(point[6].y), color);
	Novice::DrawLine(int(point[5].x), int(point[5].y), int(point[7].x), int(point[7].y), color);
	Novice::DrawLine(int(point[6].x), int(point[6].y), int(point[7].x), int(point[7].y), color);*/


}
