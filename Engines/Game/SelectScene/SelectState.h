#pragma once
#include "Camera.h"

class SelectScene;

/// <summary>
/// インゲームの状態管理基底クラス
/// </summary>
class ISelectState
{
public:
	ISelectState() = default;
	virtual ~ISelectState() = default;

	virtual void Initialize() = 0;

	virtual void Update() = 0;

	virtual void Draw(Camera* camera) = 0;

protected:

	SelectScene* selectScene_ = nullptr;

};

/// <summary>
/// 通常のゲームシーン更新
/// </summary>
class SelectAct : public ISelectState
{
public:
	SelectAct() {};
	~SelectAct() override {};

	void Initialize() override;

	void Update() override;

	void Draw(Camera* camera) override;

private:

};

/// <summary>
/// ポーズ画面更新
/// </summary>
class SelectPose : public ISelectState
{
public:
	SelectPose() {};
	~SelectPose() override {};

	void Initialize() override;

	void Update() override;

	void Draw(Camera* camera) override;

private:

};

