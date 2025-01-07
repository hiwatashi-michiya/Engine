#include "Game.h"
#include "Game/SceneFactory.h"
#include "SceneChangeManager.h"

void Game::Initialize() {
	//エンジン側のフレームワーク初期化
	MarFrameWork::Initialize();

	//ゲームシーンのインスタンス生成
	sceneFactory_ = std::make_unique<SceneFactory>();
	SceneManager::GetInstance()->SetSceneFactory(sceneFactory_.get());
	SceneManager::GetInstance()->ChangeScene("TITLE");
	SceneChangeManager::GetInstance()->Initialize();

}

void Game::Finalize() {
	//終了処理
	MarFrameWork::Finalize();

}

void Game::Update() {
	//シーンの更新
	sceneManager_->Update();
	//シーン切り替え演出更新
	SceneChangeManager::GetInstance()->Update();

}

void Game::Draw() {
	//シーン描画コマンド追加
	sceneManager_->Draw();
	//シーン切り替え演出描画コマンド追加
	SceneChangeManager::GetInstance()->Draw();
	//実際の描画
	sceneManager_->Render();

}



