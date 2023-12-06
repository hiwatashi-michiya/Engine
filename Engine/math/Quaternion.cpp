#include "Quaternion.h"
#include <functional>
#include <cmath>

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

Quaternion operator*(const Quaternion& qr1, const Quaternion& qr2) { return Multiply(qr1, qr2); }