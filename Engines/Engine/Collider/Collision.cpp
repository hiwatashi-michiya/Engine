#include "Matrix4x4.h"
#include "Collision.h"
#include <math.h>
#include <algorithm>
#include <cassert>
#include <vector>

bool IsCollision(const Sphere& s1, const Sphere& s2) {

	//中心点間の距離を求める
	float distance = Length(Subtract(s2.center, s1.center));
	//衝突判定
	if (distance <= s1.radius + s2.radius) {
		return true;
	}

	return false;

}

bool IsCollision(const Sphere& sphere, const Plane& plane) {

	//平面と球の中心点の距離を求める
	float distance = fabsf(Dot(plane.normal, sphere.center) - plane.distance);

	if (distance <= sphere.radius) {
		return true;
	}

	return false;

}

bool IsCollision(const Segment& segment, const Plane& plane) {

	//垂直判定を行うために法線と線の内積を求める
	float dot = Dot(plane.normal, segment.diff);

	//垂直なら当たっていない
	if (dot == 0.0f) {
		return false;
	}

	//媒介変数tを求める
	float t = (plane.distance - Dot(segment.origin, plane.normal)) / dot;

	//tが定義域内なら衝突している
	if (t >= 0.0f and t <= 1.0f) {
		return true;
	}

	return false;

}

bool IsCollision(const Segment& segment, const Triangle& triangle) {

	//各辺を結ぶベクトル
	Vector3 v1 = Subtract(triangle.vertices[1], triangle.vertices[0]);
	Vector3 v2 = Subtract(triangle.vertices[2], triangle.vertices[1]);
	Vector3 v3 = Subtract(triangle.vertices[0], triangle.vertices[2]);

	//法線nを求める
	Vector3 normal = Cross(v1, v2);

	normal = Normalize(normal);

	//距離を求める
	float distance = Dot(triangle.vertices[0], normal);

	//垂直判定を行うために法線と線の内積を求める
	float dot = Dot(normal, segment.diff);

	//垂直なら当たっていない
	if (dot == 0.0f) {
		return false;
	}

	//媒介変数tを求める
	float t = (distance - Dot(segment.origin, normal)) / dot;

	//tが定義域外なら当たっていない
	if (t < 0.0f || t > 1.0f) {
		return false;
	}

	//衝突点の計算
	Vector3 point = Add(segment.origin, Multiply(t, segment.diff));

	//衝突点と3頂点の差分ベクトル
	Vector3 vp1 = Subtract(point, triangle.vertices[1]);
	Vector3 vp2 = Subtract(point, triangle.vertices[2]);
	Vector3 vp3 = Subtract(point, triangle.vertices[0]);

	//クロス積
	Vector3 cross10 = Cross(v1, vp1);
	Vector3 cross21 = Cross(v2, vp2);
	Vector3 cross02 = Cross(v3, vp3);

	//全ての小三角形のクロス積と法線が同じ方向を向いていたら衝突
	if (Dot(cross10, normal) >= 0.0f and
		Dot(cross21, normal) >= 0.0f and
		Dot(cross02, normal) >= 0.0f) {
		return true;
	}

	return false;

}

bool IsCollision(const AABB& aabb1, const AABB& aabb2) {

	if ((aabb1.min.x < aabb2.max.x and aabb1.max.x > aabb2.min.x) and
		(aabb1.min.y < aabb2.max.y and aabb1.max.y > aabb2.min.y) and
		(aabb1.min.z < aabb2.max.z and aabb1.max.z > aabb2.min.z)) {
		return true;
	}

	return false;

}

bool IsCollision(const AABB& aabb, const Sphere& sphere) {

	//最接近点を求める
	Vector3 closestPoint{
		std::clamp(sphere.center.x, aabb.min.x, aabb.max.x),
		std::clamp(sphere.center.y, aabb.min.y, aabb.max.y),
		std::clamp(sphere.center.z, aabb.min.z, aabb.max.z),
	};

	//最接近点と球の中心との座標を求める
	float distance = Length(Subtract(closestPoint, sphere.center));

	//距離が半径よりも小さければ衝突
	if (distance <= sphere.radius) {
		return true;
	}


	return false;

}

bool IsCollision(const AABB& aabb, const Segment& segment) {


	//それぞれの媒介変数を求める
	float tMinX = (aabb.min.x - segment.origin.x) / segment.diff.x;
	float tMaxX = (aabb.max.x - segment.origin.x) / segment.diff.x;
	float tMinY = (aabb.min.y - segment.origin.y) / segment.diff.y;
	float tMaxY = (aabb.max.y - segment.origin.y) / segment.diff.y;
	float tMinZ = (aabb.min.z - segment.origin.z) / segment.diff.z;
	float tMaxZ = (aabb.max.z - segment.origin.z) / segment.diff.z;

	//衝突点の近い方と遠い方を求める
	float tNearX = std::min(tMinX, tMaxX);
	float tFarX = std::max(tMinX, tMaxX);
	float tNearY = std::min(tMinY, tMaxY);
	float tFarY = std::max(tMinY, tMaxY);
	float tNearZ = std::min(tMinZ, tMaxZ);
	float tFarZ = std::max(tMinZ, tMaxZ);

	//AABBとの衝突点(貫通点)のtが小さい方
	float tMin = std::max(std::max(tNearX, tNearY), tNearZ);
	//AABBとの衝突点(貫通点)のtが大きい方
	float tMax = std::min(std::min(tFarX, tFarY), tFarZ);

	//衝突しているかどうか判定
	if (tMin <= tMax and
		((tMin >= 0.0f and tMin <= 1.0f) ||
			(tMax >= 0.0f and tMax <= 1.0f))) {
		return true;
	}

	return false;

}

bool IsCollision(const OBB& obb, const Sphere& sphere) {

	//衝突判定のための変数を設定
	Matrix4x4 obbWorldMatrix{
		{
		{obb.orientations[0].x,obb.orientations[0].y,obb.orientations[0].z,0,},
		{obb.orientations[1].x,obb.orientations[1].y,obb.orientations[1].z,0,},
		{obb.orientations[2].x,obb.orientations[2].y,obb.orientations[2].z,0,},
		{obb.center.x, obb.center.y,obb.center.z, 1}
		}
	};

	Matrix4x4 obbWorldMatrixInverse = Inverse(obbWorldMatrix);

	Vector3 centerInOBBLocalSpace = CoordTransform(sphere.center, obbWorldMatrixInverse);

	AABB aabbOBBLocal{ Vector3(-obb.size.x, -obb.size.y, -obb.size.z), obb.size };
	Sphere sphereOBBLocal{ centerInOBBLocalSpace, sphere.radius };
	//ローカル空間で衝突判定
	if (IsCollision(aabbOBBLocal, sphereOBBLocal)) {
		return true;
	}

	return false;

}

bool IsCollision(const Sphere& sphere, const OBB& obb) {

	return IsCollision(obb, sphere);

}

bool IsCollision(const OBB& obb, const Segment& segment) {

	//衝突判定のための変数を設定
	Matrix4x4 obbWorldMatrix{
		{
		{obb.orientations[0].x,obb.orientations[0].y,obb.orientations[0].z,0,},
		{obb.orientations[1].x,obb.orientations[1].y,obb.orientations[1].z,0,},
		{obb.orientations[2].x,obb.orientations[2].y,obb.orientations[2].z,0,},
		{obb.center.x, obb.center.y,obb.center.z, 1}
		}
	};

	Matrix4x4 obbWorldMatrixInverse = Inverse(obbWorldMatrix);

	Vector3 localOrigin = CoordTransform(segment.origin, obbWorldMatrixInverse);
	Vector3 localEnd = CoordTransform(Add(segment.origin, segment.diff), obbWorldMatrixInverse);

	AABB localAABB{
		{-obb.size.x, -obb.size.y, -obb.size.z},
		{obb.size.x, obb.size.y, obb.size.z}
	};

	Segment localSegment;
	localSegment.origin = localOrigin;
	localSegment.diff = Subtract(localEnd, localOrigin);

	return IsCollision(localAABB, localSegment);

}

bool IsCollision(const Segment& segment, const OBB& obb) {

	return IsCollision(obb, segment);

}

//OBB同士の当たり判定
bool IsCollision(const OBB& obb1, const OBB& obb2) {

	//面法線判定
	for (uint32_t i = 0; i < 6; i++) {

		Vector3 axis;

		//軸を抽出
		if (i < 3) {
			axis = obb1.orientations[i];
		}
		else {
			axis = obb2.orientations[i - 3];
		}

		Vector3 tmpSize1[3];
		tmpSize1[0] = obb1.orientations[0] * obb1.size.x;
		tmpSize1[1] = obb1.orientations[1] * obb1.size.y;
		tmpSize1[2] = obb1.orientations[2] * obb1.size.z;

		//分離軸に対して射影
		float point1[8];
		point1[0] = Dot(obb1.center + tmpSize1[0] + tmpSize1[1] + tmpSize1[2], axis);
		point1[1] = Dot(obb1.center + tmpSize1[0] - tmpSize1[1] + tmpSize1[2], axis);
		point1[2] = Dot(obb1.center + tmpSize1[0] + tmpSize1[1] - tmpSize1[2], axis);
		point1[3] = Dot(obb1.center + tmpSize1[0] - tmpSize1[1] - tmpSize1[2], axis);
		point1[4] = Dot(obb1.center - tmpSize1[0] + tmpSize1[1] + tmpSize1[2], axis);
		point1[5] = Dot(obb1.center - tmpSize1[0] - tmpSize1[1] + tmpSize1[2], axis);
		point1[6] = Dot(obb1.center - tmpSize1[0] + tmpSize1[1] - tmpSize1[2], axis);
		point1[7] = Dot(obb1.center - tmpSize1[0] - tmpSize1[1] - tmpSize1[2], axis);

		float min1 = point1[0];
		float max1 = point1[0];

		//最小値と最大値を求める
		for (uint32_t k = 0; k < 8; k++) {

			if (min1 > point1[k]) {
				min1 = point1[k];
			}

			if (max1 < point1[k]) {
				max1 = point1[k];
			}

		}

		//差分を求める
		float length1 = max1 - min1;

		Vector3 tmpSize2[3];
		tmpSize2[0] = obb2.orientations[0] * obb2.size.x;
		tmpSize2[1] = obb2.orientations[1] * obb2.size.y;
		tmpSize2[2] = obb2.orientations[2] * obb2.size.z;

		//分離軸に対して射影
		float point2[8];
		point2[0] = Dot(obb2.center + tmpSize2[0] + tmpSize2[1] + tmpSize2[2], axis);
		point2[1] = Dot(obb2.center + tmpSize2[0] - tmpSize2[1] + tmpSize2[2], axis);
		point2[2] = Dot(obb2.center + tmpSize2[0] + tmpSize2[1] - tmpSize2[2], axis);
		point2[3] = Dot(obb2.center + tmpSize2[0] - tmpSize2[1] - tmpSize2[2], axis);
		point2[4] = Dot(obb2.center - tmpSize2[0] + tmpSize2[1] + tmpSize2[2], axis);
		point2[5] = Dot(obb2.center - tmpSize2[0] - tmpSize2[1] + tmpSize2[2], axis);
		point2[6] = Dot(obb2.center - tmpSize2[0] + tmpSize2[1] - tmpSize2[2], axis);
		point2[7] = Dot(obb2.center - tmpSize2[0] - tmpSize2[1] - tmpSize2[2], axis);

		float min2 = point2[0];
		float max2 = point2[0];

		//最小値と最大値を求める
		for (uint32_t k = 0; k < 8; k++) {

			if (min2 > point2[k]) {
				min2 = point2[k];
			}

			if (max2 < point2[k]) {
				max2 = point2[k];
			}

		}

		//差分を求める
		float length2 = max2 - min2;

		//それぞれのobbの端から端までの長さを足す
		float sumSpan = length1 + length2;
		//二つのobbの位置の端から端までを計算
		float longSpan = std::max(max1, max2) - std::min(min1, min2);

		//sumSpanがlongSpanより短い場合、隙間があることになるので分離しているといえる
		if (sumSpan < longSpan) {
			return false;
		}

	}

	Vector3 axises[9];
	axises[0] = Cross(obb1.orientations[0], obb2.orientations[0]);
	axises[1] = Cross(obb1.orientations[0], obb2.orientations[1]);
	axises[2] = Cross(obb1.orientations[0], obb2.orientations[2]);
	axises[3] = Cross(obb1.orientations[1], obb2.orientations[0]);
	axises[4] = Cross(obb1.orientations[1], obb2.orientations[1]);
	axises[5] = Cross(obb1.orientations[1], obb2.orientations[2]);
	axises[6] = Cross(obb1.orientations[2], obb2.orientations[0]);
	axises[7] = Cross(obb1.orientations[2], obb2.orientations[1]);
	axises[8] = Cross(obb1.orientations[2], obb2.orientations[2]);

	for (uint32_t i = 0; i < 9; i++) {

		Vector3 tmpSize1[3];
		tmpSize1[0] = obb1.orientations[0] * obb1.size.x;
		tmpSize1[1] = obb1.orientations[1] * obb1.size.y;
		tmpSize1[2] = obb1.orientations[2] * obb1.size.z;

		//分離軸に対して射影
		float point1[8];
		point1[0] = Dot(obb1.center + tmpSize1[0] + tmpSize1[1] + tmpSize1[2], axises[i]);
		point1[1] = Dot(obb1.center + tmpSize1[0] - tmpSize1[1] + tmpSize1[2], axises[i]);
		point1[2] = Dot(obb1.center + tmpSize1[0] + tmpSize1[1] - tmpSize1[2], axises[i]);
		point1[3] = Dot(obb1.center + tmpSize1[0] - tmpSize1[1] - tmpSize1[2], axises[i]);
		point1[4] = Dot(obb1.center - tmpSize1[0] + tmpSize1[1] + tmpSize1[2], axises[i]);
		point1[5] = Dot(obb1.center - tmpSize1[0] - tmpSize1[1] + tmpSize1[2], axises[i]);
		point1[6] = Dot(obb1.center - tmpSize1[0] + tmpSize1[1] - tmpSize1[2], axises[i]);
		point1[7] = Dot(obb1.center - tmpSize1[0] - tmpSize1[1] - tmpSize1[2], axises[i]);

		float min1 = point1[0];
		float max1 = point1[0];

		//最小値と最大値を求める
		for (uint32_t k = 0; k < 8; k++) {

			if (min1 > point1[k]) {
				min1 = point1[k];
			}

			if (max1 < point1[k]) {
				max1 = point1[k];
			}

		}

		//差分を求める
		float length1 = max1 - min1;

		Vector3 tmpSize2[3];
		tmpSize2[0] = obb2.orientations[0] * obb2.size.x;
		tmpSize2[1] = obb2.orientations[1] * obb2.size.y;
		tmpSize2[2] = obb2.orientations[2] * obb2.size.z;

		//分離軸に対して射影
		float point2[8];
		point2[0] = Dot(obb2.center + tmpSize2[0] + tmpSize2[1] + tmpSize2[2], axises[i]);
		point2[1] = Dot(obb2.center + tmpSize2[0] - tmpSize2[1] + tmpSize2[2], axises[i]);
		point2[2] = Dot(obb2.center + tmpSize2[0] + tmpSize2[1] - tmpSize2[2], axises[i]);
		point2[3] = Dot(obb2.center + tmpSize2[0] - tmpSize2[1] - tmpSize2[2], axises[i]);
		point2[4] = Dot(obb2.center - tmpSize2[0] + tmpSize2[1] + tmpSize2[2], axises[i]);
		point2[5] = Dot(obb2.center - tmpSize2[0] - tmpSize2[1] + tmpSize2[2], axises[i]);
		point2[6] = Dot(obb2.center - tmpSize2[0] + tmpSize2[1] - tmpSize2[2], axises[i]);
		point2[7] = Dot(obb2.center - tmpSize2[0] - tmpSize2[1] - tmpSize2[2], axises[i]);

		float min2 = point2[0];
		float max2 = point2[0];

		//最小値と最大値を求める
		for (uint32_t k = 0; k < 8; k++) {

			if (min2 > point2[k]) {
				min2 = point2[k];
			}

			if (max2 < point2[k]) {
				max2 = point2[k];
			}

		}

		//差分を求める
		float length2 = max2 - min2;

		//それぞれのobbの端から端までの長さを足す
		float sumSpan = length1 + length2;
		//二つのobbの位置の端から端までを計算
		float longSpan = std::max(max1, max2) - std::min(min1, min2);

		//sumSpanがlongSpanより短い場合、隙間があることになるので分離しているといえる
		if (sumSpan < longSpan) {
			return false;
		}

	}

	return true;

}

bool IsCollision(const OBB& obb, const Vector3& point)
{

	Vector3 v = point - obb.center;

	//各軸についてチェック
	for (int32_t i = 0; i < 3; i++) {
		//射影の絶対値計算
		float proj = Dot(v, obb.orientations[i]);

		if (i == 0) {
			if (std::abs(proj) > obb.size.x) {
				return false;
			}
		}
		else if (i == 1) {
			if (std::abs(proj) > obb.size.y) {
				return false;
			}
		}
		else {
			if (std::abs(proj) > obb.size.z) {
				return false;
			}
		}

	}

	return true;
}

bool IsWrapped(const OBB& outSide, const OBB& inside)
{

	std::vector<Vector3> points = {
		{inside.center.x - inside.size.x, inside.center.y - inside.size.y, inside.center.z - inside.size.z},
		{inside.center.x - inside.size.x, inside.center.y - inside.size.y, inside.center.z + inside.size.z},
		{inside.center.x - inside.size.x, inside.center.y + inside.size.y, inside.center.z - inside.size.z},
		{inside.center.x - inside.size.x, inside.center.y + inside.size.y, inside.center.z + inside.size.z},
		{inside.center.x + inside.size.x, inside.center.y - inside.size.y, inside.center.z - inside.size.z},
		{inside.center.x + inside.size.x, inside.center.y - inside.size.y, inside.center.z + inside.size.z},
		{inside.center.x + inside.size.x, inside.center.y + inside.size.y, inside.center.z - inside.size.z},
		{inside.center.x + inside.size.x, inside.center.y + inside.size.y, inside.center.z + inside.size.z},
	};

	//全ての点との当たり判定について
	for (int32_t i = 0; i < points.size(); i++) {
		//1つでも外にあれば完全に内包されていない
		if (not IsCollision(outSide, points[i])) {
			return false;
		}

	}

	return true;
}

