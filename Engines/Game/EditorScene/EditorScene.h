#pragma once
#include "Audio/AudioManager.h"
#include "base/DirectXSetter.h"
#include "Drawing/TextureManager.h"
#include "Game/DefaultScene.h"
#include "input/Input.h"
#include "UniqueEditor.h"
#include "Model.h"
#include "Particle.h"
#include "Particle3D.h"
#include "Sprite.h"
#include "Tool/GlobalVariables.h"
#include "Transform.h"
#include <memory>
#include "DebugCamera.h"

class EditorScene : public DefaultScene
{
public:
	EditorScene();
	~EditorScene() override;

	void Initialize() override;

	void Update() override;

	void DrawModel() override;

	void DrawSkinningModel() override;

	void DrawParticle() override;

	void DrawSprite() override;

	void DrawLine() override;

private:

	UniqueEditor* editor_ = nullptr;

	std::unique_ptr<Camera> camera_;

	std::unique_ptr<DebugCamera> debugCamera_;

	std::unique_ptr<Model> model_;

	std::unique_ptr<Transform> transform_;

	std::unique_ptr<Sprite> test_;

	Texture* tex_;

};
