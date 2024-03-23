#include "Engine/Leak.h"
static D3DResourceLeakChecker leak;
#include "Engine/Engine.h"
#include "Engine/input/Input.h"
#include "MolFrameWork.h"

void MolFrameWork::Initialize() {

	Engine::Initialize("カラコロ", 1280, 720);

	sceneManager_ = SceneManager::GetInstance();

}

void MolFrameWork::Finalize() {

	Engine::Finalize();

}

void MolFrameWork::Update() {

	sceneManager_->Update();

}

void MolFrameWork::Run() {

	Initialize();

	while (Engine::ProcessMessage() == 0)
	{

		//フレーム開始
		Engine::BeginFrame();

		//ゲームシーン更新
		Update();

		if (Input::GetInstance()->TriggerKey(DIK_ESCAPE)) {
			break;
		}

		//ゲームシーン描画
		Draw();

		//フレーム終了
		Engine::EndFrame();

	}

	Finalize();

}
