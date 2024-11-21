#include <Windows.h>
#include "Game/Game.h"
#include <memory>

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	std::unique_ptr<MarFrameWork> game = std::make_unique<Game>();
	game->Run();

	return 0;

}