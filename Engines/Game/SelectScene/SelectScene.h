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
#include "Skinning/SkinningModel.h"
#include "Drawing/LineDrawer.h"

class SelectScene : public DefaultScene
{
public:
	SelectScene();
	~SelectScene() override;

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

	float animationTime_ = 0.0f;

	Matrix4x4 localMatrix_{};

	std::unique_ptr<SkinningModel> model_;

	std::unique_ptr<Transform> transform_;

	float speed_ = 1.0f;

	uint32_t test_;

	std::unique_ptr<Line> line_;

};
