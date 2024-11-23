#pragma once
#include "Vector2.h"

class Player;

/// <summary>
/// 停止時の更新処理
/// </summary>
class PlayerStay
{
public:
	PlayerStay();
	~PlayerStay();

	void InitializeStay(Player& player);

	void UpdateStay(Player& player);

private:



};

/// <summary>
/// 移動時の更新処理
/// </summary>
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

/// <summary>
/// 弾発射時の更新処理
/// </summary>
class PlayerShot
{
public:
	PlayerShot();
	~PlayerShot();

	void InitializeShot(Player& player);

	void UpdateShot(Player& player);

private:



};





