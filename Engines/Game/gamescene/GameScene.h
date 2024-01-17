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
#include "math/Quaternion.h"

class GameScene
{
public:
	GameScene();
	~GameScene();

	void Initialize();

	void Update();

	void Draw();

private:

	Input* input_ = nullptr;

	std::unique_ptr<Camera> camera_;	

	Quaternion rotation0;
	Quaternion rotation1;

	Quaternion interpolate0;
	Quaternion interpolate1;
	Quaternion interpolate2;
	Quaternion interpolate3;
	Quaternion interpolate4;

};


