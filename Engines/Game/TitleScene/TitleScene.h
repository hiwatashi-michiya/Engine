#pragma once
#include "Audio/AudioManager.h"
#include "base/DirectXSetter.h"
#include "Drawing/TextureManager.h"
#include "Game/DefaultScene.h"
#include "input/Input.h"
#include "MapEditor.h"
#include "Model.h"
#include "Particle.h"
#include "Particle3D.h"
#include "Sprite.h"
#include "Tool/GlobalVariables.h"
#include "Transform.h"
#include <memory>

class TitleScene : public DefaultScene
{
public:
	TitleScene();
	~TitleScene() override;

	void Initialize() override;

	void Update() override;

	void DrawModel() override;

	void DrawSkinningModel() override;

	void DrawParticle() override;

	void DrawSprite() override;

	void DrawLine() override;

private:

	MapEditor* editor_ = nullptr;

	std::unique_ptr<Camera> camera_;

	std::unique_ptr<Model> model_;

	std::unique_ptr<Transform> transform_;

};
