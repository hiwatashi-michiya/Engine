#include "Engine/Leak.h"
static D3DResourceLeakChecker leak;
#include "Engine/Engine.h"
#include "Engine/input/Input.h"
#include "MarFrameWork.h"

void MarFrameWork::Initialize() {

	Engine::Initialize("カイロウ", 1280, 720);

	sceneManager_ = SceneManager::GetInstance();

}

void MarFrameWork::Finalize() {

	Engine::Finalize();

}

void MarFrameWork::Update() {

	sceneManager_->Update();

}

void MarFrameWork::Run() {

	Initialize();

	while (true)
	{

		//フレーム開始
		Engine::BeginFrame();

		//ゲームシーン更新
		Update();

		//エスケープキーが押されるか、ウィンドウのxボタンが押されたら終了する
		if (Input::GetInstance()->TriggerKey(DIK_ESCAPE) or Engine::ProcessMessage() != 0) {
			sceneManager_->Finalize();
			break;
		}

		//ゲームシーン描画
		Draw();

		//フレーム終了
		Engine::EndFrame();

	}

	Finalize();

}
