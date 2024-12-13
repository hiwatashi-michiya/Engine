#pragma once
#include "FrameWork/MarFrameWork.h"

/// <summary>
/// ゲームの流れを更新するクラス
/// </summary>
class Game : public MarFrameWork {
public:

	//初期化
	void Initialize() override;
	//終了処理
	void Finalize() override;
	//更新
	void Update() override;
	//描画
	void Draw() override;

private:

	

};
