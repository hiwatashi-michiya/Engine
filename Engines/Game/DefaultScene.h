#pragma once
#include "Audio/AudioManager.h"
#include "Core/DirectXSetter.h"
#include "Drawing/TextureManager.h"
#include "FrameWork/BaseScene.h"
#include "input/Input.h"
#include "Editor/MapEditor.h"
#include "Model.h"
#include "Particle.h"
#include "Particle3D.h"
#include "Sprite.h"
#include "Tool/GlobalVariables.h"
#include "Transform.h"
#include <memory>
#include "SceneChangeManager.h"
#include "ParticleManager.h"

/// <summary>
/// ゲーム固有のシーン間共通の変数を持つクラス
/// </summary>
class DefaultScene : public BaseScene
{
public:
	

protected:

	DirectXSetter* dxSetter_ = nullptr;
	Input* input_ = nullptr;
	AudioManager* audioManager_ = nullptr;
	ParticleManager* particleManager_ = nullptr;
	//現在のステージ
	static uint32_t stageNumber_;
	//最大ステージ数
	static const uint32_t kMaxStage_ = 5;

};
