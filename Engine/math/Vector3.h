#pragma once

/// <summary>
/// 3次元ベクトル
/// </summary>
struct Vector3 {

	float x;
	float y;
	float z;

	Vector3& operator*=(float s) { x *= s; y *= s; z *= s; return *this; }
	Vector3& operator-=(const Vector3& v) { x -= v.x;  y -= v.y; z -= v.z; return *this; }
	Vector3& operator+=(const Vector3& v) { x += v.x;  y += v.y; z += v.z; return *this; }
	Vector3& operator/=(float s) { x /= s; y /= s; z /= s; return *this; }

};

class WorldTransform;

struct Transform {
	Vector3 scale;
	Vector3 rotate;
	Vector3 translate;
};

// 直線
struct Line {
	Vector3 origin; // 始点
	Vector3 diff;   // 終点への差分ベクトル
};

// 半直線
struct Ray {
	Vector3 origin; // 始点
	Vector3 diff;   // 終点への差分ベクトル
};

// 線分
struct Segment {
	Vector3 origin; // 始点
	Vector3 diff;   // 終点への差分ベクトル
};

// 三角形
struct Triangle {
	Vector3 vertices[3]; // 頂点
};

// AABB(箱)
struct AABB {
	Vector3 min; // 最小点
	Vector3 max; // 最大点
};

// OBB(箱)
class OBB {
public:

	Vector3 center;          // 中心点
	Vector3 orientations[3]; // 座標軸。正規化・直交必須
	Vector3 size;            // 座標軸方向の長さの半分。中心から面までの距離

	void SetOBB(const WorldTransform& worldTransform, float scale = 1.0f,
		Vector3 offset = {0.0f,0.0f,0.0f});

};

//バネ構造体
struct Spring {
	//アンカー。固定された端の位置
	Vector3 anchor;
	float naturalLength; //自然長
	float stiffness; //剛性。バネ定数k
	float dampingCoefficient; //減衰係数
};

//ボール構造体(3次元)
struct Ball {
	Vector3 position; //位置
	Vector3 velocity; //速度
	Vector3 acceleration; //加速度
	float mass; //ボールの質量
	float radius; // ボールの半径
	unsigned int color; //色
};

//振り子構造体
struct Pendulum {
	Vector3 anchor; //アンカー。固定された端の位置
	float length; //紐の長さ
	float angle; //現在の角度
	float angularVelocity; //角速度ω
	float angularAcceleration; //角加速度
};

//円錐振り子構造体
struct ConicalPendulum {
	Vector3 anchor; // アンカー。固定された端の位置
	float length; // 紐の長さ
	float halfApexAngle; //円錐の頂角の半分
	float angle; // 現在の角度
	float angularVelocity; // 角速度ω
};

//カプセル構造体
struct Capsule {
	Segment segment;
	float radius;
};

struct Matrix4x4;

Vector3 Add(const Vector3& v1, const Vector3& v2);

Vector3 Subtract(const Vector3& v1, const Vector3& v2);

Vector3 Multiply(float scalar, const Vector3& v1);

float Dot(const Vector3& v1, const Vector3& v2);

float Length(const Vector3& v);

Vector3 Normalize(const Vector3& v);

//座標変換
Vector3 CoordTransform(const Vector3& vector, const Matrix4x4& matrix);

//ベクトル変換(スケールと回転)
Vector3 TransformNormal(const Vector3& v, const Matrix4x4& m);

// クロス積
Vector3 Cross(const Vector3& v1, const Vector3& v2);

// 線形補間
Vector3 Lerp(const Vector3& v1, const Vector3& v2, float t);

// 球面線形補間
Vector3 Slerp(const Vector3& v1, const Vector3& v2, float t);

Vector3 CatmullRomPoint(const Vector3& p0, const Vector3& p1, const Vector3& p2, const Vector3& p3, float t);

float Clamp(float x, float min, float max);

Vector3 Project(const Vector3& v1, const Vector3& v2);
Vector3 ClosestPoint(const Vector3& point, const Segment& segment);

// 垂直なベクトルを求める関数
Vector3 Perpendicular(const Vector3& vector);

//反射ベクトル
Vector3 Reflect(const Vector3& input, const Vector3& normal);

void ClampAABB(AABB& aabb);

// OBB回転
void RotateOBB(OBB& obb, const Vector3& rotate);

//二項演算子
Vector3 operator+(const Vector3& v1, const Vector3& v2);
Vector3 operator-(const Vector3& v1, const Vector3& v2);
Vector3 operator*(float s, const Vector3& v);
Vector3 operator*(const Vector3& v, float s);
Vector3 operator/(const Vector3& v, float s);

//単項演算子
Vector3 operator-(const Vector3& v);
Vector3 operator+(const Vector3& v);
