#include "Game.h"

void Game::Initialize() {

	MolFrameWork::Initialize();

	//ゲームシーンのインスタンス生成
	scene_ = new GameScene();
	scene_->Initialize();

}

void Game::Finalize() {

	delete scene_;

	MolFrameWork::Finalize();

}

void Game::Update() {

	scene_->Update();

}

void Game::Draw() {

	scene_->Draw();

}



