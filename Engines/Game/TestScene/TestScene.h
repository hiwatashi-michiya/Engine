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

	MapEditor* editor_ = nullptr;
	//ローダー
	LevelDataLoader* loader_ = nullptr;

	std::unique_ptr<Camera> camera_;
	//モデルとトランスフォームリスト
	std::vector<std::unique_ptr<Model>> models_;
	std::vector<std::unique_ptr<Transform>> transforms_;

	//線分
	std::unique_ptr<Line> line_;

	Texture* testTex_ = nullptr;

};
