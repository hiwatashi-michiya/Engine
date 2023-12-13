#include "Quaternion.h"
#include <functional>
#include <cmath>
#include <algorithm>

Quaternion::Quaternion()
{
	//単位クォータニオンで初期化
	*this = IdentityQuaternion();
}

Quaternion::Quaternion(float x, float y, float z, float w) : 
	x(x), y(y), z(z), w(w)
{

}

Quaternion Quaternion::Multiply(const Quaternion& hs) {

	Quaternion qr;

	qr.w = this->w * hs.w - this->x * hs.x - this->y * hs.y - this->z * hs.z;
	qr.x = this->y * hs.z - this->z * hs.y + hs.w * this->x + this->w * hs.x;
	qr.y = this->z * hs.x - this->x * hs.z + hs.w * this->y + this->w * hs.y;
	qr.z = this->x * hs.y - this->y * hs.x + hs.w * this->z + this->w * hs.z;

	*this = qr;

	return qr;

}

Quaternion Quaternion::IdentityQuaternion() {

	*this = { 0.0f,0.0f,0.0f,1.0f };

	return *this;

}

Quaternion Quaternion::ConjuGate() {

	Quaternion qr;

	qr.w = this->w;
	qr.x = -this->x;
	qr.y = -this->y;
	qr.z = -this->z;

	*this = qr;

	return qr;

}

float Quaternion::Norm() {

	float result;

	result = std::sqrtf(std::powf(this->w, 2) + std::powf(this->x, 2)+
		std::powf(this->y, 2) + std::powf(this->z, 2));

	return result;

}

Quaternion Quaternion::Normalize() {

	float norm = this->Norm();

	Quaternion qr;

	qr.w = this->w;
	qr.x = this->x;
	qr.y = this->y;
	qr.z = this->z;

	if (norm != 0.0f) {

		qr.w = this->w / norm;
		qr.x = this->x / norm;
		qr.y = this->y / norm;
		qr.z = this->z / norm;
	}

	*this = qr;

	return qr;

}

Quaternion Quaternion::Inverse() {

	Quaternion qr = this->ConjuGate();

	float squaredNorm = std::powf(this->Norm(), 2);

	if (squaredNorm != 0.0f) {

		qr.w = qr.w / squaredNorm;
		qr.x = qr.x / squaredNorm;
		qr.y = qr.y / squaredNorm;
		qr.z = qr.z / squaredNorm;

	}

	*this = qr;

	return qr;

}

Quaternion Quaternion::MakeRotateAxisAngleQuaternion(const Vector3& axis, float angle) {

	Quaternion qr;

	qr.w = std::cosf(angle / 2.0f);
	qr.x = axis.x * std::sinf(angle / 2.0f);
	qr.y = axis.y * std::sinf(angle / 2.0f);
	qr.z = axis.z * std::sinf(angle / 2.0f);

	*this = qr;

	return qr;

}

//球面線形補間
Quaternion Quaternion::Slerp(const Quaternion& qr, float t) {

	t = std::clamp(t, 0.0f, 1.0f);

	Quaternion q0 = *this;
	Quaternion q1 = qr;

	float dot = q0.x * q1.x + q0.y * q1.y + q0.z * q1.z + q0.w * q1.w;

	//遠ければもう片方の回転を利用
	if (dot < 0.0f) {

		q0 = -q0;
		dot = -dot;

	}

	//為す角を求める
	float theta = std::acosf(dot);

	float scale0 = std::sinf((1.0f - t) * theta) / std::sinf(theta);
	float scale1 = std::sinf(t * theta) / std::sinf(theta);

	return scale0 * q0 + scale1 * q1;

}

Quaternion Multiply(const Quaternion& lhs, const Quaternion& rhs) {

	Quaternion qr;

	qr.w = lhs.w * rhs.w - lhs.x * rhs.x - lhs.y * rhs.y - lhs.z * rhs.z;
	qr.x = lhs.y * rhs.z - lhs.z * rhs.y + rhs.w * lhs.x + lhs.w * rhs.x;
	qr.y = lhs.z * rhs.x - lhs.x * rhs.z + rhs.w * lhs.y + lhs.w * rhs.y;
	qr.z = lhs.x * rhs.y - lhs.y * rhs.x + rhs.w * lhs.z + lhs.w * rhs.z;

	return qr;

}

Quaternion ConjuGate(const Quaternion& quaternion) {

	Quaternion qr;

	qr.w = quaternion.w;
	qr.x = -quaternion.x;
	qr.y = -quaternion.y;
	qr.z = -quaternion.z;

	return qr;

}

float Norm(const Quaternion& quaternion) {

	float result;

	result = std::sqrtf(std::powf(quaternion.w, 2) + std::powf(quaternion.x, 2) +
		std::powf(quaternion.y, 2) + std::powf(quaternion.z, 2));

	return result;

}

Quaternion Normalize(const Quaternion& quaternion) {

	float norm = Norm(quaternion);

	Quaternion qr;

	qr.w = quaternion.w;
	qr.x = quaternion.x;
	qr.y = quaternion.y;
	qr.z = quaternion.z;

	if (norm != 0.0f) {

		qr.w = quaternion.w / norm;
		qr.x = quaternion.x / norm;
		qr.y = quaternion.y / norm;
		qr.z = quaternion.z / norm;
	}

	return qr;

}

Quaternion Inverse(const Quaternion& quaternion) {

	Quaternion qr = ConjuGate(quaternion);

	float squaredNorm = std::powf(Norm(quaternion), 2);

	if (squaredNorm != 0.0f) {

		qr.w = qr.w / squaredNorm;
		qr.x = qr.x / squaredNorm;
		qr.y = qr.y / squaredNorm;
		qr.z = qr.z / squaredNorm;

	}

	return qr;

}

Quaternion MakeRotateAxisAngleQuaternion(const Vector3& axis, float angle) {

	Quaternion qr;

	qr.w = std::cosf(angle / 2.0f);
	qr.x = axis.x * std::sinf(angle / 2.0f);
	qr.y = axis.y * std::sinf(angle / 2.0f);
	qr.z = axis.z * std::sinf(angle / 2.0f);

	return qr;

}

//球面線形補間
Quaternion Slerp(const Quaternion& qr0, const Quaternion& qr1, float t) {

	t = std::clamp(t, 0.0f, 1.0f);

	Quaternion q0 = Normalize(qr0);
	Quaternion q1 = Normalize(qr1);

	float dot = q0.x * q1.x + q0.y * q1.y + q0.z * q1.z + q0.w * q1.w;

	//遠ければもう片方の回転を利用
	if (dot < 0.0f) {

		q0 = -q0;
		dot = -dot;

	}

	//零に近い値
	float epsilon = 0.0005f;

	//特異点に近い場合、sinを使わずに処理
	if (dot >= 1.0f - epsilon) {
		return (1.0f - t) * q0 + t * q1;
	}


	//為す角を求める
	float theta = std::acosf(dot);

	float scale0 = std::sinf((1.0f - t) * theta) / std::sinf(theta);
	float scale1 = std::sinf(t * theta) / std::sinf(theta);

	return scale0 * q0 + scale1 * q1;

}

Quaternion operator*(const Quaternion& qr1, const Quaternion& qr2) { return Multiply(qr1, qr2); }

Quaternion operator*(float val, const Quaternion& qr) {
	return Quaternion(qr.x * val, qr.y * val, qr.z * val, qr.w * val);
}

Quaternion operator*(const Quaternion& qr, float val) {
	return Quaternion(qr.x * val, qr.y * val, qr.z * val, qr.w * val);
}

Quaternion operator+(const Quaternion& qr1, const Quaternion& qr2) {
	return Quaternion(qr1.x + qr2.x, qr1.y + qr2.y, qr1.z + qr2.z, qr1.w + qr2.w);
}
Quaternion operator-(const Quaternion& qr) { return Quaternion(-qr.x, -qr.y, -qr.z, -qr.w); }
