#include "MyMath.h"
#include <cmath>
#include <algorithm>

//最短角度補間
float LerpShortAngle(float a, float b, float t) {

	t = std::clamp(t, 0.0f, 1.0f);

	//float diff = b - a;

	////角度を+-2πに補正
	//std::fmodf(6.28f, diff);

	////+πを上回っていれば、-2πする
	//if (diff >= 3.14f) {
	//	diff -= 6.28f;
	//}
	//
	////-πを上回っていれば、+2πする
	//else if (diff < -3.14f) {
	//	diff += 6.28f;
	//}

	return std::lerp(a, b, t);

}

float Atan2(float y, float x) {

	y = std::clamp(y, -1.0f, 1.0f);
	x = std::clamp(x, -1.0f, 1.0f);

	//未定義の動作を事前に防ぐ
	if (x == 0.0f) {
		return 0.0f;
	}

	float result;

	float pi = 3.14f;

	if (y >= 0) {

		result = (1.0f - x) * pi / 2.0f;

	}
	else {

		result = -(1.0f - x) * pi / 2.0f;

	}

	return result;

}
