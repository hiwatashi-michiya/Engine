#pragma once
#include "AbsSceneFactory.h"
#include "SceneManager.h"
#include <memory>

/// <summary>
/// フレームワーク
/// </summary>
class MarFrameWork {
public:

	virtual ~MarFrameWork() = default;

	//初期化
	virtual void Initialize();
	//終了処理
	virtual void Finalize();
	//更新
	virtual void Update();
	//描画
	virtual void Draw() = 0;
	//ゲームの流れまとめ
	void Run();

	//終了チェック
	virtual bool IsEndMessage() { return endMessage_; }

protected:

	std::unique_ptr<AbsSceneFactory> sceneFactory_ = nullptr;

	SceneManager* sceneManager_ = nullptr;

private:
	//終了フラグ
	bool endMessage_ = false;

};
