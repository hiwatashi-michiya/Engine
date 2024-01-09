#pragma once
#include "Engine/base/DirectXCommon.h"
#include "Engine/manager/TextureManager.h"
#include "Engine/3d/Model.h"
#include "Engine/3d/WorldTransform.h"
#include "Engine/input/Input.h"
#include "Engine/2d/Sprite.h"
#include <memory>
#include "Game/player/Player.h"
#include "Game/enemy/Enemy.h"
#include "Game/stage/Stage.h"
#include "Engine/3d/Particle3D.h"
#include "Engine/manager/AudioManager.h"
#include "Engine/Tool/GlobalVariables.h"
#include "Engine/Tool/MapEditor.h"
#include "Engine/Tool/Particle.h"

class GameScene
{
public:
	GameScene();
	~GameScene();

	void Initialize();

	void Update();

	void Draw();

	enum Scene {
		kTitle,
		kSelect,
		kGame
	};

private:

	void TitleUpdate();

	void SelectUpdate();

	void GameUpdate();

	void TitleInitialize();

	void SelectInitialize();

	void GameInitialize();

	void TitleDraw();

	void SelectDraw();

	void GameDraw();

	void SceneChange();

private:

	DirectXCommon* dxCommon_ = nullptr;
	Input* input_ = nullptr;
	AudioManager* audioManager_ = nullptr;
	GlobalVariables* gv_ = nullptr;
	MapEditor* mapEditor_ = nullptr;

	std::unique_ptr<Camera> camera_;	

	std::unique_ptr<Stage> stage_;

	std::unique_ptr<Model> skydome_;

	Scene currentScene_ = kTitle;

	Scene nextScene_ = kTitle;

	bool isInitialize_ = false;

	bool isSceneChange_ = false;

	//ゲーム中クリアしたかどうか
	bool isClear_ = false;

	const uint32_t kMaxStage_ = 3;

	uint32_t stageNumber_ = 1;

	Texture* blackTex_ = nullptr;

	std::unique_ptr<Sprite> fade_;

	Texture* stage1Tex_;
	Texture* stage2Tex_;
	Texture* stage3Tex_;
	Texture* selectTileTex_;

	Texture* push_A_Tex_;
	Texture* moveTex_;
	Texture* selectTex_;
	Texture* stageSelectTex_;
	Texture* stageClearTex_;
	Texture* titleTex_;
	Texture* backTex_;

	std::unique_ptr<Sprite> stage1Sprite_;
	std::unique_ptr<Sprite> stage2Sprite_;
	std::unique_ptr<Sprite> stage3Sprite_;
	std::unique_ptr<Sprite> selectTileSprite_;

	std::unique_ptr<Sprite> push_A_Sprite_;
	std::unique_ptr<Sprite> moveSprite_;
	std::unique_ptr<Sprite> selectSprite_;
	std::unique_ptr<Sprite> stageSelectSprite_;
	std::unique_ptr<Sprite> stageClearSprite_;
	std::unique_ptr<Sprite> titleSprite_;
	std::unique_ptr<Sprite> backSprite_;

};


