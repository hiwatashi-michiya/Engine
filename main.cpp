#include "Engine/Leak.h"
#include "Engine/Engine.h"
#include "Engine/input/Input.h"
#include <Windows.h>
#include "Engine/manager/TextureManager.h"
#include <memory>
#include "Game/gamescene/GameScene.h"

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	D3DResourceLeakChecker leak;

	Engine::Initialize("engine", 1280, 720);

	//ゲームシーンのインスタンス生成
	GameScene* gameScene = new GameScene();
	gameScene->Initialize();

	while (Engine::ProcessMessage() == 0)
	{

		//フレーム開始
		Engine::BeginFrame();

		//ゲームシーン更新
		gameScene->Update();
		
		//ゲームシーン描画
		gameScene->Draw();

		//フレーム終了
		Engine::EndFrame();

	}

	delete gameScene;

	TextureManager::GetInstance()->Finalize();

	Engine::Finalize();

	return 0;

}