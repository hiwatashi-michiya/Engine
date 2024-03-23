#pragma once
#include "base/DirectXSetter.h"
#include "Drawing/TextureManager.h"
#include "Model.h"
#include "input/Input.h"
#include "Sprite.h"
#include <memory>
#include "Particle3D.h"
#include "Audio/AudioManager.h"
#include "Tool/GlobalVariables.h"
#include "MapEditor.h"
#include "Particle.h"
#include "FrameWork/BaseScene.h"
#include "Transform.h"

class DefaultScene : public BaseScene
{
public:
	

protected:

	DirectXSetter* dxSetter_ = nullptr;
	Input* input_ = nullptr;
	AudioManager* audioManager_ = nullptr;

	

};
