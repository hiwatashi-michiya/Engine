#pragma once
#include "Vector4.h"
#include "Vector3.h"
#include <stdint.h>
#include <string>
#include <array>

/// <summary>
/// ゲームの色を数字に置き換える
/// </summary>
namespace GameColor {

	enum Color {

		kWhite, //白色
		kRed, //赤色
		kGreen, //緑色
		kBlue, //青色

		kMaxColor

	};
	//色の名前
	static std::array<const char*, kMaxColor> colorNames = {
	"White", "Red", "Green", "Blue"
	};

	//数字から色を作る
	Vector4 CreateColor(const GameColor::Color& color);
	//数字から色を作る(Vector3)
	Vector3 CreateVector3Color(const GameColor::Color& color);
	//ステージの色とホルダーの色を取得して対象のオブジェクトがアクティブ状態にあるかどうかを判定
	bool CheckIsActiveColor(const GameColor::Color& color, const GameColor::Color& stageColor);

};
