#include "MyEngine.h"
#include "Input.h"
#include <Windows.h>
#include "TextureManager.h"

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	MyEngine::Initialize("engine", 1280, 720);

	Input* input = nullptr;
	
	TextureManager::GetInstance()->Initialize();

	Texture tex;
	tex = TextureManager::GetInstance()->Load("resources/monsterBall.png");


	input = Input::GetInstance();
	input->Initialize();

	while (MyEngine::ProcessMessage() == 0)
	{

		//フレーム開始
		MyEngine::BeginFrame();

		input->Update();

		if (input->TriggerKey(DIK_0)) {
			OutputDebugStringA("Hit 0\n");
		}

		//フレーム終了
		MyEngine::EndFrame();

	}

	MyEngine::Finalize();

	return 0;

}