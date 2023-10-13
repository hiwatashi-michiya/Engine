#include "Engine/Engine.h"
#include "Engine/input/Input.h"
#include <Windows.h>
#include "Engine/base/TextureManager.h"
#include <memory>

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	D3DResourceLeakChecker leak;

	Engine::Initialize("engine", 1280, 720);

	Input* input = nullptr;
	
	TextureManager::GetInstance()->Initialize();

	Texture tex;
	tex = TextureManager::GetInstance()->Load("resources/monsterBall.png");

	std::unique_ptr<Sprite> sprite = nullptr;

	sprite.reset(Sprite::Create(tex, { 100.0f,0.0f }));

	input = Input::GetInstance();
	input->Initialize();

	while (Engine::ProcessMessage() == 0)
	{

		//フレーム開始
		Engine::BeginFrame();

		input->Update();

		if (input->TriggerKey(DIK_0)) {
			OutputDebugStringA("Hit 0\n");
		}

		sprite->Draw();

		//フレーム終了
		Engine::EndFrame();

	}

	TextureManager::GetInstance()->Finalize();

	Engine::Finalize();

	return 0;

}