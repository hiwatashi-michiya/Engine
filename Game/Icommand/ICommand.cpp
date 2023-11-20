#include "ICommand.h"
#include "Game/player/Player.h"

ICommand::ICommand()
{
}

ICommand::~ICommand()
{
}

void MoveRightCommand::Exec(Player& player) {
	player.MoveRight();
}

void MoveLeftCommand::Exec(Player& player) {
	player.MoveLeft();
}
