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
	case GameColor::kGreen:
		return { 0.0f,1.0f,0.0f,1.0f };
		break;
	case GameColor::kBlue:
		return { 0.0f,0.0f,1.0f,1.0f };
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
	case GameColor::kGreen:
		return { 0.0f,1.0f,0.0f };
		break;
	case GameColor::kBlue:
		return { 0.0f,0.0f,1.0f };
		break;
	
	}

}

bool GameColor::CheckIsActiveColor(GameColor::Color color, GameColor::Color stageColor)
{

	//色が白色ならtrue
	if (color == kWhite) {
		return true;
	}

	//どちらかの色と一致しているならtrue
	if (color == stageColor) {
		return true;
	}

	return false;

}
