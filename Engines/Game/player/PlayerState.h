#pragma once
#include "Vector2.h"
#include "Game/Variables/CommonVariables.h"
#include "Vector3.h"
#include "Input.h"

class Player;

/// <summary>
/// PlayerのStatePattern基底クラス
/// </summary>
class PlayerState
{
public:

	PlayerState() = default;
	virtual ~PlayerState() = default;

	virtual void Initialize(Player* player) = 0;

	virtual void Update() = 0;

protected:

	Input* input_ = nullptr;
	//プレイヤーのポインタ
	Player* player_ = nullptr;

};


/// <summary>
/// 停止時の更新処理
/// </summary>
class PlayerStay : public PlayerState
{
public:

	PlayerStay() {};

	~PlayerStay() override {};

	void Initialize(Player* player) override;

	void Update() override;

private:



};

/// <summary>
/// 移動時の更新処理
/// </summary>
class PlayerMove : public PlayerState
{
public:

	PlayerMove() {};

	~PlayerMove() override {};

	void Initialize(Player* player) override;

	void Update() override;

private:

	Vector2 moveVelocity_{};

};

/// <summary>
/// 弾発射時の更新処理
/// </summary>
class PlayerShot : public PlayerState
{
public:

	PlayerShot() {};

	~PlayerShot() override {};

	void Initialize(Player* player) override;

	void Update() override;

private:



};

/// <summary>
/// 潜行時の更新処理
/// </summary>
class PlayerDive : public PlayerState
{
public:

	PlayerDive() {};

	~PlayerDive() override {};

	void Initialize(Player* player) override;

	void Update() override;

private:

	//潜行中の速度計算
	Vector3 CalcDiveVelocity(const Vector3& vec);

private:

	//潜行の初期時間
	static const int32_t startDiveCount_ = 30;

};




