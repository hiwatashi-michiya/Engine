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
