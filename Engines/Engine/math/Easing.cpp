#include "Easing.h"
#include <cmath>
#include <numbers>

float Easing::InSine(float x)
{
	return 1.0f - std::cosf((x * float(std::numbers::pi)) * 0.5f);
}

float Easing::InCubic(float x)
{
	return x * x * x;
}

float Easing::InQuint(float x)
{
	return x * x * x * x * x;
}

float Easing::InCirc(float x)
{
	return 1.0f - std::sqrtf(1.0f - std::powf(x, 2));
}

float Easing::InElastic(float x)
{
	const float c4 = (2.0f * float(std::numbers::pi)) * 0.33333333f;
	
	if (x == 0.0f or x == 1.0f) {
		return x;
	}
	
	return -powf(2.0f, 10.0f * x - 10.0f) * std::sinf((x * 10.0f - 10.75f) * c4);
}

float Easing::InQuad(float x)
{
	return x * x;
}

float Easing::InQuart(float x)
{
	return x * x * x * x;
}

float Easing::InExpo(float x)
{

	if (x == 0.0f) {
		return x;
	}

	return std::powf(2.0f, 10.0f * x - 10.0f);
}

float Easing::InBack(float x)
{
	const float c1 = 1.70158f;
	const float c3 = c1 + 1.0f;
	return c3 * x * x * x - c1 * x * x;
}

float Easing::InBounce(float x)
{
	return Easing::OutBounce(1.0f - x);
}

float Easing::OutSine(float x)
{
	return std::sinf((x * float(std::numbers::pi)) * 0.5f);
}

float Easing::OutCubic(float x)
{
	return 1.0f - std::powf(1.0f - x, 3);
}

float Easing::OutQuint(float x)
{
	return 1.0f - std::powf(1.0f - x, 5);
}

float Easing::OutCirc(float x)
{
	return std::sqrtf(1.0f - std::powf(x - 1.0f, 2));
}

float Easing::OutElastic(float x)
{
	const float c4 = (2.0f * float(std::numbers::pi)) * 0.33333333f;

	if (x == 0.0f or x == 1.0f) {
		return x;
	}

	return std::powf(2.0f, -10.0f * x) * std::sinf((x * 10.0f - 0.75f) * c4) + 1.0f;
}

float Easing::OutQuad(float x)
{
	return 1.0f - (1.0f - x) * (1.0f - x);
}

float Easing::OutQuart(float x)
{
	return 1.0f - std::powf(1.0f - x, 4);
}

float Easing::OutExpo(float x)
{

	if (x == 1.0f) {
		return x;
	}

	return 1.0f - std::powf(2.0f, -10.0f * x);
}

float Easing::OutBack(float x)
{
	const float c1 = 1.70158f;
	const float c3 = c1 + 1.0f;
	return 1.0f + c3 * std::powf(x - 1.0f, 3) + c1 * std::powf(x - 1.0f, 2);
}

float Easing::OutBounce(float x)
{
	const float n1 = 7.5625f;
	const float d1 = 2.75f;

	if (x < 1.0f / d1) {
		return n1 * x * x;
	}
	else if (x < 2.0f / d1) {
		return n1 * (x -= 1.5f / d1) * x + 0.75f;
	}
	else if (x < 2.5f / d1) {
		return n1 * (x -= 2.25f / d1) * x + 0.9375f;
	}
	else {
		return n1 * (x -= 2.625f / d1) * x + 0.984375f;
	}

}

float Easing::InOutSine(float x)
{
	return -(std::cosf(float(std::numbers::pi) * x) - 1.0f) * 0.5f;
}

float Easing::InOutCubic(float x)
{
	return x < 0.5f ?
		4.0f * x * x * x :
		1.0f - std::powf(-2.0f * x + 2.0f, 3) * 0.5f;
}

float Easing::InOutQuint(float x)
{
	return x < 0.5f ?
		16.0f * x * x * x * x * x :
		1.0f - std::powf(-2.0f * x + 2.0f, 5) * 0.5f;
}

float Easing::InOutCirc(float x)
{
	return x < 0.5f ?
		(1.0f - std::sqrtf(1.0f - std::powf(2.0f * x, 2))) * 0.5f :
		(std::sqrtf(1.0f - std::powf(-2.0f * x + 2.0f, 2)) + 1.0f) * 0.5f;
}

float Easing::InOutElastic(float x)
{
	const float c5 = (2.0f * float(std::numbers::pi)) / 4.5f;

	if (x == 0.0f or x == 1.0f) {
		return x;
	}

	return x < 0.5f ?
		-(std::powf(2.0f, 20.0f * x - 10.0f) * std::sinf((20.0f * x - 11.125f) * c5)) * 0.5f :
		(std::powf(2.0f, -20.0f * x + 10.0f) * std::sinf((20.0f * x - 11.125f) * c5)) * 0.5f + 1.0f;
}

float Easing::InOutQuad(float x)
{
	return x < 0.5f ?
		2.0f * x * x :
		1.0f - std::powf(-2.0f * x + 2.0f, 2) * 0.5f;
}

float Easing::InOutQuart(float x)
{
	return x < 0.5f ?
		8.0f * x * x * x * x :
		1.0f - std::powf(-2.0f * x + 2.0f, 4) * 0.5f;
}

float Easing::InOutExpo(float x)
{

	if (x == 0.0f or x == 1.0f) {
		return x;
	}

	return x < 0.5f ?
		std::powf(2.0f, 20.0f * x - 10.0f) * 0.5f :
		(2.0f - std::powf(2.0f, -20.0f * x + 10.0f)) * 0.5f;
}

float Easing::InOutBack(float x)
{
	const float c1 = 1.70158f;
	const float c2 = c1 * 1.525f;

	return x < 0.5f ?
		(std::powf(2.0f * x, 2) * ((c2 + 1.0f) * 2.0f * x - c2)) * 0.5f :
		(std::powf(2.0f * x - 2.0f, 2) * ((c2 + 1.0f) * (x * 2.0f - 2.0f) + c2) + 2.0f) * 0.5f;
}

float Easing::InOutBounce(float x)
{
	return x < 0.5f ?
		(1.0f - Easing::OutBounce(1.0f - 2.0f * x)) * 0.5f :
		(1.0f + Easing::OutBounce(2.0f * x - 1.0f)) * 0.5f;
}
