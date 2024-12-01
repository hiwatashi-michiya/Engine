#pragma once

/// <summary>
/// ゲーム内で使用する共通変数の定義
/// </summary>
namespace CommonVariables {

	/// <summary>
	/// 回転方向
	/// </summary>
	enum class RotateType {
		kClockwise, //時計回り
		kCounterclockwise, //反時計回り
	};

	/// <summary>
	/// 現在のカメラ視点
	/// </summary>
	enum class CameraType {
		kSide, //横視点
		kAbove, //上視点
	};

}
