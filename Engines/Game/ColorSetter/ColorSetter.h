#pragma once
#include "Vector4.h"
#include "Vector3.h"
#include <stdint.h>
#include <string>
#include <array>

namespace GameColor {

	enum Color {


		kWhite, //白色
		kRed, //赤色
		kBlue, //青色
		kGreen, //緑色
		kYellow, //黄色
		kCyan, //空色
		kMagenta, //紫色

		kMaxColor

	};

	static std::array<const char*, kMaxColor> colorNames = {
	"White", "Red", "Blue", "Green", "Yellow", "Cyan", "Magenta"
	};

	//数字から色を作る
	Vector4 CreateColor(GameColor::Color color);

	Vector3 CreateVector3Color(GameColor::Color color);

};
