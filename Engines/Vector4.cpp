#include "Vector4.h"
#include <algorithm>


Vector4 Lerp(const Vector4& v1, const Vector4& v2, float t)
{

	t = std::clamp(t, 0.0f, 1.0f);

	Vector4 p = Vector4(
		v1.x + t * (v2.x - v1.x),
		v1.y + t * (v2.y - v1.y),
		v1.z + t * (v2.z - v1.z),
		v1.w + t * (v2.w - v1.w)
	);

	return p;

}
