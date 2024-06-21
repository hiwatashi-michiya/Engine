#pragma once
#include "Vector3.h"

struct Sphere
{
	Vector3 center;
	float radius;
};

struct Plane {
	Vector3 normal; //法線
	float distance; //距離
};

//球と球の当たり判定
bool IsCollision(const Sphere& s1, const Sphere& s2);

//球と平面の当たり判定
bool IsCollision(const Sphere& sphere, const Plane& plane);

//線分と平面の当たり判定
bool IsCollision(const Segment& segment, const Plane& plane);

//線分と三角形の当たり判定
bool IsCollision(const Segment& segment, const Triangle& triangle);

//AABBの当たり判定
bool IsCollision(const AABB& aabb1, const AABB& aabb2);

//球とAABBの当たり判定
bool IsCollision(const AABB& aabb, const Sphere& sphere);

//AABBと線分の当たり判定
bool IsCollision(const AABB& aabb, const Segment& segment);

//OBBと球の当たり判定
bool IsCollision(const OBB& obb, const Sphere& sphere);

bool IsCollision(const Sphere& sphere, const OBB& obb);

//OBBと線分の当たり判定
bool IsCollision(const OBB& obb, const Segment& segment);

bool IsCollision(const Segment& segment, const OBB& obb);

//OBB同士の当たり判定
bool IsCollision(const OBB& obb1, const OBB& obb2);
