#pragma once
#include "Audio/AudioManager.h"
#include "base/DirectXSetter.h"
#include "Drawing/TextureManager.h"
#include "FrameWork/BaseScene.h"
#include "input/Input.h"
#include "MapEditor.h"
#include "Model.h"
#include "Particle.h"
#include "Particle3D.h"
#include "Sprite.h"
#include "Tool/GlobalVariables.h"
#include "Transform.h"
#include <memory>
#include "SceneChangeManager.h"
#include "ParticleManager.h"

class DefaultScene : public BaseScene
{
public:
	

protected:

	DirectXSetter* dxSetter_ = nullptr;
	Input* input_ = nullptr;
	AudioManager* audioManager_ = nullptr;
	ParticleManager* particleManager_ = nullptr;

	static uint32_t stageNumber_;

	static const uint32_t kMaxStage_ = 7;

};
