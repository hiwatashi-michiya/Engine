#pragma once
#include "Animation/Animation.h"
#include "Editor/MapEditor.h"
#include "Audio/AudioManager.h"
#include "Core/DirectXSetter.h"
#include "Drawing/LineDrawer.h"
#include "Drawing/TextureManager.h"
#include "Game/DefaultScene.h"
#include "input/Input.h"
#include "Model.h"
#include "Particle.h"
#include "Particle3D.h"
#include "Skinning/SkinningModel.h"
#include "Sprite.h"
#include "Tool/GlobalVariables.h"
#include "Transform.h"
#include <memory>
#include "Tool/LevelDataLoader.h"
#include "DebugCamera.h"

/// <summary>
/// 新機能を試すためのシーン
/// </summary>
class TestScene : public DefaultScene
{
public:
	TestScene();
	~TestScene() override;

	void Initialize() override;

	void Finalize() override;

	void Update() override;

	void Draw() override;

private:

	std::unique_ptr<DebugCamera> camera_;

	std::unique_ptr<Sprite> testA_;
	std::unique_ptr<Sprite> testB_;
	std::unique_ptr<Sprite> testC_;
	std::unique_ptr<Model> testD_;
	std::unique_ptr<Model> testE_;

	std::unique_ptr<Transform> testDTransform_;
	std::unique_ptr<Transform> testETransform_;


	Texture* testATex_ = nullptr;
	Texture* testBTex_ = nullptr;
	Texture* testCTex_ = nullptr;
	Texture* testDTex_ = nullptr;
	Texture* testETex_ = nullptr;

};
