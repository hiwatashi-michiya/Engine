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
#include "Game/DefaultScene.h"
#include "Transform.h"
#include "Application/MapEditor.h"
#include "Animation/Animation.h"

class SelectScene : public DefaultScene
{
public:
	SelectScene();
	~SelectScene() override;

	void Initialize() override;

	void Update() override;

	void Draw() override;

private:

	MapEditor* editor_ = nullptr;

	std::unique_ptr<Camera> camera_;

	float animationTime_ = 0.0f;

	Matrix4x4 localMatrix_{};

	std::unique_ptr<Model> model_;
	std::unique_ptr<Animation> animation_;

};
