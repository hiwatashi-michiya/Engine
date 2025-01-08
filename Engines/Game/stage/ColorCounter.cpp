#include "ColorCounter.h"
#include "Game/Gimmick/GhostBox.h"

ColorCounter::ColorCounter()
{
	
	numTex_ = TextureManager::GetInstance()->Load("./Resources/UI/num.png");
	blockTex_ = TextureManager::GetInstance()->Load("./Resources/block/ghostBox.png");
	slashTex_ = TextureManager::GetInstance()->Load("./Resources/UI/slash.png");

	for (int32_t i = 0; i < GameColor::kMaxColor - 1; i++) {

		for (int32_t k = 0; k < 2; k++) {
			countNumSprites_[i][k].reset(Sprite::Create(numTex_, {100.0f + 48.0f * k, 50.0f + 64.0f * i}));
			countNumSprites_[i][k]->size_ = { 64.0f,64.0f };
			countNumSprites_[i][k]->viewRect_ = { 0.1f,1.0f };
			goalNumSprites_[i][k].reset(Sprite::Create(numTex_, { 250.0f + 48.0f * k, 50.0f + 64.0f * i }));
			goalNumSprites_[i][k]->size_ = { 64.0f,64.0f };
			goalNumSprites_[i][k]->viewRect_ = { 0.1f,1.0f };
		}

		blockSprites_[i].reset(Sprite::Create(blockTex_, { 50.0f, 50.0f + 64.0f * i }));
		slashSprites_[i].reset(Sprite::Create(slashTex_, { 200.0f, 50.0f + 64.0f * i }));

	}

	blockSprites_[GameColor::kRed - 1]->color_ = { 1.0f,0.0f,0.0f,1.0f };
	blockSprites_[GameColor::kGreen - 1]->color_ = { 0.0f,1.0f,0.0f,1.0f };
	blockSprites_[GameColor::kBlue - 1]->color_ = { 0.0f,0.0f,1.0f,1.0f };

}

ColorCounter::~ColorCounter()
{
}

void ColorCounter::Initialize()
{
}

void ColorCounter::Update()
{

	for (int32_t i = 0; i < GameColor::kMaxColor - 1; i++) {
		//カウントリセット
		currentCount_[i] = 0;
	}

	for (auto& box : GhostBox::boxList_) {

		//赤カウント
		if (box->GetColor() == GameColor::kRed) {
			currentCount_[GameColor::kRed - 1]++;
		}

		//緑カウント
		if (box->GetColor() == GameColor::kGreen) {
			currentCount_[GameColor::kGreen - 1]++;
		}

		//青カウント
		if (box->GetColor() == GameColor::kBlue) {
			currentCount_[GameColor::kBlue - 1]++;
		}

	}

	
	for (int32_t i = 0; i < GameColor::kMaxColor - 1; i++) {

		//10分割するために定義
		float division = 0.1f;

		//割る数字
		int32_t divideNum = int32_t(std::pow(10, 2 - 1));

		//現在の数字
		int32_t currentNum = currentCount_[i];

		for (int32_t k = 0; k < 2; k++) {

			if (divideNum == 0) {
				break;;
			}

			//描画する数字
			int32_t setNum = currentNum / divideNum;

			//上の方の桁から計算し、UVセット
			countNumSprites_[i][k]->uvTranslate_.x = float(setNum) * division;

			//次の桁に移行する
			currentNum = currentNum % divideNum;
			divideNum = int32_t(divideNum * 0.1f);

			//カウントが一致していたらスプライトを緑に、違ったら赤にする
			if (currentCount_[i] == goalCount_[i]) {
				countNumSprites_[i][k]->color_ = { 0.0f,1.0f,0.0f,1.0f };
			}
			else {
				countNumSprites_[i][k]->color_ = { 1.0f,0.0f,0.0f,1.0f };
			}

		}

	}

}

void ColorCounter::Draw()
{

	for (int32_t i = 0; i < GameColor::kMaxColor - 1; i++) {
		//ブロック描画
		blockSprites_[i]->Draw();

		//スラッシュ描画
		slashSprites_[i]->Draw();

		//十以上の時に二桁目描画
		if (currentCount_[i] >= 10) {
			countNumSprites_[i][0]->Draw();
		}
		//現在の数字描画
		countNumSprites_[i][1]->Draw();
		//十以上の時に二桁目描画
		if (goalCount_[i] >= 10) {
			goalNumSprites_[i][0]->Draw();
		}
		//ゴールカウント描画
		goalNumSprites_[i][1]->Draw();
	}

}

void ColorCounter::SetGoalCount(int32_t rNum, int32_t gNum, int32_t bNum)
{

	goalCount_[GameColor::kRed - 1] = rNum;
	goalCount_[GameColor::kGreen - 1] = gNum;
	goalCount_[GameColor::kBlue - 1] = bNum;

	for (int32_t i = 0; i < GameColor::kMaxColor - 1; i++) {

		//10分割するために定義
		float division = 0.1f;

		//割る数字
		int32_t divideNum = int32_t(std::pow(10, 2 - 1));

		//現在の数字
		int32_t currentNum = goalCount_[i];

		for (int32_t k = 0; k < 2; k++) {

			if (divideNum == 0) {
				break;
			}

			//描画する数字
			int32_t setNum = currentNum / divideNum;

			//上の方の桁から計算し、UVセット
			goalNumSprites_[i][k]->uvTranslate_.x = float(setNum) * division;

			//次の桁に移行する
			currentNum = currentNum % divideNum;
			divideNum = int32_t(divideNum * 0.1f);

		}

	}

}

bool ColorCounter::IsAllCountComplete()
{

	for (int32_t i = 0; i < GameColor::kMaxColor - 1; i++) {

		//1つでも合っていなかったらfalse
		if (currentCount_[i] != goalCount_[i]) {
			return false;
		}

	}

	return true;
}

