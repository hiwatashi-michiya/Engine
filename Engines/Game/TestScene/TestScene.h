#pragma once
#include "Animation/Animation.h"
#include "Application/MapEditor.h"
#include "Audio/AudioManager.h"
#include "DirectXSetter.h"
#include "Drawing/LineDrawer.h"
#include "Drawing/TextureManager.h"
#include "Game/DefaultScene.h"
#include "input/Input.h"
#include "MapEditor.h"
#include "Model.h"
#include "Particle.h"
#include "Particle3D.h"
#include "Skinning/SkinningModel.h"
#include "Sprite.h"
#include "Tool/GlobalVariables.h"
#include "Transform.h"
#include <memory>
#include "Tool/LevelDataLoader.h"

class TestScene : public DefaultScene
{
public:
	TestScene();
	~TestScene() override;

	void Initialize() override;

	void Update() override;

	void DrawModel() override;

	void DrawSkinningModel() override;

	void DrawParticle() override;

	void DrawSprite() override;

	void DrawLine() override;

private:

	MapEditor* editor_ = nullptr;

	LevelDataLoader* loader_ = nullptr;

	std::unique_ptr<Camera> camera_;

	float animationTime_ = 0.0f;

	Matrix4x4 localMatrix_{};

	std::vector<std::shared_ptr<Model>> models_;
	std::vector<std::shared_ptr<Transform>> transforms_;

	float speed_ = 1.0f;

	uint32_t test_;

	std::unique_ptr<Line> line_;

};
