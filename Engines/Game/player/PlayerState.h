#pragma once
#include "Vector2.h"

class Player;

class PlayerStay
{
public:
	PlayerStay();
	~PlayerStay();

	void InitializeStay(Player& player);

	void UpdateStay(Player& player);

private:



};

class PlayerMove
{
public:
	PlayerMove();
	~PlayerMove();

	void InitializeMove(Player& player);

	void UpdateMove(Player& player);

private:

	Vector2 moveVelocity_{};

};

class PlayerShot
{
public:
	PlayerShot();
	~PlayerShot();

	void InitializeShot(Player& player);

	void UpdateShot(Player& player);

private:



};





