#pragma once
#include "Engine/base/DirectXCommon.h"
#include "Engine/manager/TextureManager.h"
#include "Engine/3d/Model.h"
#include "Engine/3d/WorldTransform.h"
#include "Engine/input/Input.h"
#include "Engine/2d/Sprite.h"
#include <memory>
#include "Game/player/Player.h"
#include "Game/enemy/Enemy.h"
#include "Game/stage/Stage.h"
#include "Engine/3d/Particle3D.h"
#include "Engine/manager/AudioManager.h"
#include "Game/InputHandler/InputHandler.h"

class GameScene
{
public:
	GameScene();
	~GameScene();

	void Initialize();

	void Update();

	void Draw();

private:

	DirectXCommon* dxCommon_ = nullptr;
	Input* input_ = nullptr;
	AudioManager* audioManager_ = nullptr;

	std::unique_ptr<InputHandler> inputHandler_ = nullptr;
	ICommand* command_ = nullptr;

	std::unique_ptr<Player> player_;

};


