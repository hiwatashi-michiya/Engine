#include "MyEngine.h"

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	MyEngine::Initialize("engine", 1280, 720);

	while (MyEngine::ProcessMessage() == 0)
	{

		//フレーム開始
		MyEngine::BeginFrame();

		//フレーム終了
		MyEngine::EndFrame();

	}

	MyEngine::Finalize();

	return 0;

}