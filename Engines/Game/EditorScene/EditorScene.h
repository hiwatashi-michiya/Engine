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
#include "Application/MapEditor.h"

class EditorScene : public BaseScene
{
public:
	EditorScene();
	~EditorScene() override;

	void Initialize() override;

	void Update() override;

	void Draw() override;

private:

	DirectXSetter* dxSetter_ = nullptr;
	Input* input_ = nullptr;
	AudioManager* audioManager_ = nullptr;
	MapEditor* editor_ = nullptr;

	std::unique_ptr<Camera> camera_;


};
