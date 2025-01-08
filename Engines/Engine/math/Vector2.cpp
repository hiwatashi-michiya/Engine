#include "Vector2.h"
#include <algorithm>

//線形補間
Vector2 Lerp(const Vector2& v1, const Vector2& v2, float t) {

	t = std::clamp(t, 0.0f, 1.0f);

	Vector2 p = Vector2(
		v1.x + t * (v2.x - v1.x),
		v1.y + t * (v2.y - v1.y)
	);

	return p;

}

