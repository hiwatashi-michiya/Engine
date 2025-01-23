#pragma once
#include <array>
#include "Sprite.h"
#include "Camera.h"
#include <memory>
#include "Game/ColorSetter/ColorSetter.h"

/// <summary>
/// ブロックが何色に染まっているかをカウントする
/// </summary>
class ColorCounter
{
public:

	ColorCounter();
	~ColorCounter();

	void Initialize();

	void Update();

	void Draw();
	//ゴールカウントセット
	void SetGoalCount(int32_t rNum, int32_t gNum, int32_t bNum);
	//全ての数字が揃っているかどうか
	bool IsAllCountComplete();

private:
	//最大桁数
	static const int32_t kMaxColumn_ = 2;

	//目標数と現在のカウント数
	std::array<std::array<std::unique_ptr<Sprite>, kMaxColumn_>, GameColor::kMaxColor - 1> countNumSprites_;
	std::array<std::array<std::unique_ptr<Sprite>, kMaxColumn_>, GameColor::kMaxColor - 1> goalNumSprites_;
	//ブロック画像
	std::array<std::unique_ptr<Sprite>, GameColor::kMaxColor - 1> blockSprites_;
	//スラッシュ画像
	std::array<std::unique_ptr<Sprite>, GameColor::kMaxColor - 1> slashSprites_;

	//ゴールカウント
	std::array<int32_t, GameColor::kMaxColor - 1> goalCount_;

	//現在のカウント
	std::array<int32_t, GameColor::kMaxColor - 1> currentCount_;

	Texture* numTex_;
	Texture* blockTex_;
	Texture* slashTex_;

};

