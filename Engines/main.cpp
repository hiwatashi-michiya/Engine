#include <Windows.h>
#include "Game/Game.h"

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	MolFrameWork* game = new Game();
	game->Run();
	delete game;

	return 0;

}