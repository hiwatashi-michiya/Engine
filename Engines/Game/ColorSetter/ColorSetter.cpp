#include "ColorSetter.h"

Vector4 GameColor::CreateColor(GameColor::Color color) {

	switch (color % GameColor::kMaxColor)
	{
	default:
	case GameColor::kWhite:
		return { 1.0f,1.0f,1.0f,1.0f };
		break;
	case GameColor::kRed:
		return { 1.0f,0.0f,0.0f,1.0f };
		break;
	case GameColor::kBlue:
		return { 0.0f,0.0f,1.0f,1.0f };
		break;
	case GameColor::kGreen:
		return { 0.0f,1.0f,0.0f,1.0f };
		break;
	case GameColor::kYellow:
		return { 1.0f,1.0f,0.0f,1.0f };
		break;
	case GameColor::kCyan:
		return { 0.0f,1.0f,1.0f,1.0f };
		break;
	case GameColor::kMagenta:
		return { 1.0f,0.0f,1.0f,1.0f };
		break;
	}


}

Vector3 GameColor::CreateVector3Color(GameColor::Color color)
{
	switch (color % GameColor::kMaxColor)
	{
	default:
	case GameColor::kWhite:
		return { 1.0f,1.0f,1.0f };
		break;
	case GameColor::kRed:
		return { 1.0f,0.0f,0.0f };
		break;
	case GameColor::kBlue:
		return { 0.0f,0.0f,1.0f };
		break;
	case GameColor::kGreen:
		return { 0.0f,1.0f,0.0f };
		break;
	case GameColor::kYellow:
		return { 1.0f,1.0f,0.0f };
		break;
	case GameColor::kCyan:
		return { 0.0f,1.0f,1.0f };
		break;
	case GameColor::kMagenta:
		return { 1.0f,0.0f,1.0f };
		break;
	}

}

bool GameColor::CheckIsActiveColor(GameColor::Color color, GameColor::Color stageColor, GameColor::Color holderColor)
{

	//色が白色ならtrue
	if (color == kWhite) {
		return true;
	}

	//どちらかの色と一致しているならtrue
	if (color == stageColor or color == holderColor) {
		return true;
	}

	//二色とも色がついている場合
	if (stageColor != kWhite and holderColor != kWhite) {

		//二色を合わせた色と同じ場合もtrue
		switch (stageColor)
		{
		case GameColor::kRed:

			if (holderColor == kBlue and color == kMagenta) {
				return true;
			}
			else if (holderColor == kGreen and color == kYellow) {
				return true;
			}

			break;
		case GameColor::kBlue:

			if (holderColor == kRed and color == kMagenta) {
				return true;
			}
			else if (holderColor == kGreen and color == kCyan) {
				return true;
			}

			break;
		case GameColor::kGreen:

			if (holderColor == kBlue and color == kCyan) {
				return true;
			}
			else if (holderColor == kRed and color == kYellow) {
				return true;
			}

			break;
		}
		//逆の組み合わせでも同様の処理を行う
		switch (holderColor)
		{
		case GameColor::kRed:

			if (stageColor == kBlue and color == kMagenta) {
				return true;
			}
			else if (stageColor == kGreen and color == kYellow) {
				return true;
			}

			break;
		case GameColor::kBlue:

			if (stageColor == kRed and color == kMagenta) {
				return true;
			}
			else if (stageColor == kGreen and color == kCyan) {
				return true;
			}

			break;
		case GameColor::kGreen:

			if (stageColor == kBlue and color == kCyan) {
				return true;
			}
			else if (stageColor == kRed and color == kYellow) {
				return true;
			}

			break;
		}

	}

	return false;

}
