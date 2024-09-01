#include "Game.h"
#include "Game/SceneFactory.h"
#include "SceneChangeManager.h"

void Game::Initialize() {

	MolFrameWork::Initialize();

	//ゲームシーンのインスタンス生成
	sceneFactory_ = std::make_unique<SceneFactory>();
	SceneManager::GetInstance()->SetSceneFactory(sceneFactory_.get());
	SceneManager::GetInstance()->ChangeScene("TITLE");
	SceneChangeManager::GetInstance()->Initialize();

}

void Game::Finalize() {

	MolFrameWork::Finalize();

}

void Game::Update() {

	sceneManager_->Update();

	SceneChangeManager::GetInstance()->Update();

}

void Game::Draw() {

	sceneManager_->Draw();

	SceneChangeManager::GetInstance()->Draw();

}



