#include <Windows.h>
#include "Game/Game.h"
#include <memory>

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
	//ゲーム生成
	std::unique_ptr<MarFrameWork> game = std::make_unique<Game>();
	//開始
	game->Run();

	return 0;

}