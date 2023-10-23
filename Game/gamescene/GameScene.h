#pragma once
#include "Engine/base/DirectXCommon.h"
#include "Engine/base/TextureManager.h"
#include "Engine/3d/Model.h"
#include "Engine/3d/WorldTransform.h"
#include "Engine/input/Input.h"
#include "Engine/2d/Sprite.h"
#include <memory>

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

	std::unique_ptr<Model> suzanneModel_;

	std::unique_ptr<Sprite> uvSprite_;

	Texture uvTex_;

	WorldTransform worldTransformSuzanne_;

};


