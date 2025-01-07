#pragma once
#include "Camera.h"
#include "GameScene.h"

/// <summary>
/// インゲームの状態管理基底クラス
/// </summary>
class IGameState
{
public:
	IGameState() = default;
	virtual ~IGameState() = default;

	virtual void Initialize() = 0;

	virtual void Update() = 0;

	virtual void Draw(Camera* camera) = 0;

protected:

	GameScene* gameScene_ = nullptr;

};

/// <summary>
/// 通常のゲームシーン更新
/// </summary>
class GameAct : public IGameState
{
public:
	GameAct() {};
	~GameAct() override {};

	void Initialize() override;

	void Update() override;

	void Draw(Camera* camera) override;

private:

};

/// <summary>
/// ポーズ画面更新
/// </summary>
class GamePose : public IGameState
{
public:
	GamePose() {};
	~GamePose() override {};

	void Initialize() override;

	void Update() override;

	void Draw(Camera* camera) override;

private:

};

