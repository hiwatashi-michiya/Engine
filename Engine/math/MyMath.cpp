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
