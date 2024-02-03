#pragma once
#include "base/DirectXCommon.h"
#include "manager/TextureManager.h"
#include "Model.h"
#include "input/Input.h"
#include "Sprite.h"
#include <memory>
#include "Particle3D.h"
#include "manager/AudioManager.h"
#include "Tool/GlobalVariables.h"
#include "MapEditor.h"
#include "Particle.h"

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

	std::unique_ptr<Camera> camera_;	

	Texture* tex_;
	Texture* tex2_;

	std::unique_ptr<Model> model_;
	std::unique_ptr<Model> model2_;

};


