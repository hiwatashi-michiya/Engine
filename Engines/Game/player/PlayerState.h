#pragma once
#include "Vector2.h"
#include "Game/Variables/CommonVariables.h"
#include "Vector3.h"

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

/// <summary>
/// 潜行時の更新処理
/// </summary>
class PlayerDive
{
public:
	PlayerDive();
	~PlayerDive();

	void InitializeDive(Player& player);

	void UpdateDive(Player& player);

public:

	//移動時の回転方向
	CommonVariables::RotateType rotateType_ = CommonVariables::RotateType::kClockwise;

	//潜っている途中かどうか
	bool isDiving_ = false;

private:
	//潜行中の速度計算
	Vector3 CalcDiveVelocity(const Vector3& vec);

};




