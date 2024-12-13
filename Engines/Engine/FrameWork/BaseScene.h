#pragma once

class SceneManager;

class BaseScene
{
public:
	//初期化
	virtual void Initialize() = 0;
	//終了処理
	virtual void Finalize() = 0;
	//更新
	virtual void Update() = 0;
	//描画
	virtual void Draw() = 0;

	virtual ~BaseScene() = default;
	//シーンマネージャーセット
	virtual void SetSceneManager(SceneManager* sceneManager) { sceneManager_ = sceneManager; }

private:

	SceneManager* sceneManager_ = nullptr;

};
