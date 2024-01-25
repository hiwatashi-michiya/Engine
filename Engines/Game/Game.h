#pragma once
#include "Game/gamescene/GameScene.h"
#include "FrameWork/MolFrameWork.h"

class Game : public MolFrameWork {
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

	GameScene* scene_;

};
