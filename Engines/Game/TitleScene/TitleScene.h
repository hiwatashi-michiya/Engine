#pragma once
#include "base/DirectXSetter.h"
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
#include "FrameWork/BaseScene.h"

class TitleScene : public BaseScene
{
public:
	TitleScene();
	~TitleScene() override;

	void Initialize() override;

	void Update() override;

	void Draw() override;

private:

	DirectXSetter* dxSetter_ = nullptr;
	Input* input_ = nullptr;
	AudioManager* audioManager_ = nullptr;

	std::unique_ptr<Camera> camera_;

	std::unique_ptr<Model> model_;

};
