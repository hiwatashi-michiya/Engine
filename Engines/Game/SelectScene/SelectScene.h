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

class SelectScene : public DefaultScene
{
public:
	SelectScene();
	~SelectScene() override;

	void Initialize() override;

	void Finalize() override;

	void Update() override;

	void Draw() override;

private:

	MapEditor* editor_ = nullptr;

	LevelDataLoader* loader_ = nullptr;

	std::unique_ptr<Camera> camera_;

	std::unique_ptr<Model> skyDome_;
	std::unique_ptr<Transform> skyDomeTransform_;

	std::unique_ptr<Sprite> stageSelect_;
	std::unique_ptr<Sprite> toTitle_;
	std::unique_ptr<Sprite> aButton_;
	std::unique_ptr<Sprite> num_;

	Texture* stageSelectTex_;
	Texture* toTitleTex_;
	Texture* buttonTex_;
	Texture* Tex1_;
	Texture* Tex2_;
	Texture* Tex3_;
	Texture* numTex_;

};
