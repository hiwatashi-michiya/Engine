#pragma once
#include "Audio/AudioManager.h"
#include "Core/DirectXSetter.h"
#include "Drawing/TextureManager.h"
#include "Game/DefaultScene.h"
#include "input/Input.h"
#include "Editor/UniqueEditor.h"
#include "Editor/ParticleEditor.h"
#include "Model.h"
#include "Particle.h"
#include "Particle3D.h"
#include "Sprite.h"
#include "Tool/GlobalVariables.h"
#include "Transform.h"
#include <memory>
#include "DebugCamera.h"

/// <summary>
/// ステージ編集などを行うシーン
/// </summary>
class EditorScene : public DefaultScene
{
public:
	EditorScene();
	~EditorScene() override;

	void Initialize() override;

	void Finalize() override;

	void Update() override;

	void Draw() override;

private:

	UniqueEditor* editor_ = nullptr;
	ParticleEditor* particleEditor_ = nullptr;
	//デバッグカメラ
	std::unique_ptr<DebugCamera> debugCamera_;

	std::unique_ptr<Sprite> test_;

	Camera* currentCamera_ = nullptr;

	Texture* tex_;

	//初期座標
	const Vector3 kStartPosition_ = { 0.0f,65.0f, -60.0f };
	//初期回転角
	const Vector3 kStartRotate_ = { 0.9f,0.0f,0.0f };

};
