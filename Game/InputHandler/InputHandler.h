#pragma once
#include "Game/Icommand/ICommand.h"

class InputHandler
{
public:
	InputHandler();
	~InputHandler();

	ICommand* HandleInput();

	void AssignMoveLeftCommand2PressKeyA();
	void AssignMoveRightCommand2PressKeyD();

private:

	ICommand* pressKeyD_;
	ICommand* pressKeyA_;

};


