#pragma once
#include "Sprite.h"
#include "TextureManager.h"
#include <memory>
#include <string>
#include "PostEffectDrawer.h"

/// <summary>
/// シーン切り替え時の動きを管理するクラス
/// </summary>
class SceneChangeManager
{
public:
	//インスタンス取得
	static SceneChangeManager* GetInstance();
	//初期化
	void Initialize();
	//シーン切り替え開始
	void SceneChangeStart();
	//更新
	void Update();
	//描画
	void Draw();
	//次のシーンをセットする
	void SetNextScene(const std::string& sceneName) { nextScene_ = sceneName; }
	//シーン切り替え開始フラグ
	bool GetIsSceneChangeEnter() const { return isSceneChangeEnter_; }
	//シーン切り替え終了フラグ
	bool GetIsSceneChangeExit() const { return isSceneChangeExit_; }

	//シーン切り替え中かどうか
	bool IsSceneChange() { return isSceneChangeEnter_ or isSceneChangeExit_; }

private:
	//ポストエフェクト描画のインスタンス
	PostEffectDrawer* postEffectDrawer_ = nullptr;
	//切り替え時の黒背景
	std::unique_ptr<Sprite> sprite_;
	//次のシーン
	std::string nextScene_;
	//シーン切り替え開始フラグ
	bool isSceneChangeEnter_ = false;
	//シーン切り替え終了フラグ
	bool isSceneChangeExit_ = false;
	//シーン切り替えの時間
	float sceneChangeValue_ = 0.0f;
	//シーン切り替え速度
	float sceneChangeSpeed_ = 0.05f;
	//シーン切り替えの最大値
	const float maxSceneChangeValue_ = 1.0f;
	//画像
	Texture* texture_ = nullptr;

private:
	//シングルトン化
	SceneChangeManager() = default;
	~SceneChangeManager() = default;
	SceneChangeManager(const SceneChangeManager&) = delete;
	const SceneChangeManager& operator=(const SceneChangeManager&) = delete;

};
