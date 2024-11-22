#pragma once

/// <summary>
/// 参考サイト
/// https://easings.net/ja
/// float型の変数をそれぞれのイージング関数で変化させる
/// </summary>
namespace Easing {

	float InSine(float x);
	float InCubic(float x);
	float InQuint(float x);
	float InCirc(float x);
	float InElastic(float x);
	float InQuad(float x);
	float InQuart(float x);
	float InExpo(float x);
	float InBack(float x);
	float InBounce(float x);

	float OutSine(float x);
	float OutCubic(float x);
	float OutQuint(float x);
	float OutCirc(float x);
	float OutElastic(float x);
	float OutQuad(float x);
	float OutQuart(float x);
	float OutExpo(float x);
	float OutBack(float x);
	float OutBounce(float x);

	float InOutSine(float x);
	float InOutCubic(float x);
	float InOutQuint(float x);
	float InOutCirc(float x);
	float InOutElastic(float x);
	float InOutQuad(float x);
	float InOutQuart(float x);
	float InOutExpo(float x);
	float InOutBack(float x);
	float InOutBounce(float x);

}