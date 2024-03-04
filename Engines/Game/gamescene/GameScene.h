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

class GameScene : public BaseScene
{
public:
	GameScene();
	~GameScene() override;

	void Initialize() override;

	void Update() override;

	void Draw() override;

private:

	DirectXSetter* dxSetter_ = nullptr;
	Input* input_ = nullptr;
	AudioManager* audioManager_ = nullptr;
	MapEditor* editor_ = nullptr;

	std::unique_ptr<Camera> camera_;

	std::unique_ptr<Model> model_;
	std::unique_ptr<Model> model2_;
	std::unique_ptr<Model> model3_;
	std::unique_ptr<Model> model4_;
	std::unique_ptr<Model> model5_;

	std::unique_ptr<Transform> transform_;
	std::unique_ptr<Transform> transform2_;
	std::unique_ptr<Transform> transform3_;
	std::unique_ptr<Transform> transform4_;
	std::unique_ptr<Transform> transform5_;

};


