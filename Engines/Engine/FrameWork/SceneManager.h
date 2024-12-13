#pragma once
#include "AbsSceneFactory.h"
#include "BaseScene.h"
#include <memory>

/// <summary>
/// シーンの切り替えや更新、描画処理を行うクラス
/// </summary>
class SceneManager
{
public:
	//インスタンス取得
	static SceneManager* GetInstance();
	//更新
	void Update();
	//終了処理
	void Finalize();
	//描画コマンドを積む
	void Draw();
	//実際の描画
	void Render();
	//シーン切り替え
	void ChangeScene(const std::string& sceneName);
	//シーンファクトリーセット
	void SetSceneFactory(AbsSceneFactory* sceneFactory) { sceneFactory_ = sceneFactory; }

private:

	AbsSceneFactory* sceneFactory_ = nullptr;
	//現在のシーン
	std::unique_ptr<BaseScene> scene_;
	//次のシーン
	std::unique_ptr<BaseScene> nextScene_;

private:
	//シングルトン化
	SceneManager() = default;
	~SceneManager() = default;
	SceneManager(const SceneManager&) = delete;
	const SceneManager& operator=(const SceneManager&) = delete;

};
