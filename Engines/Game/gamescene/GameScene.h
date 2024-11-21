#pragma once
#include "Game/DefaultScene.h"
#include "Game/player/Player.h"
#include "Game/stage/Stage.h"
#include "Transform.h"
#include <array>
#include <memory>
#include "Skybox.h"
#include "Game/Camera/FollowCamera.h"

class GameScene : public DefaultScene
{
public:
	GameScene();
	~GameScene() override;

	void Initialize() override;

	void Finalize() override;

	void Update() override;

	void Draw() override;

private:


private:

	DirectXSetter* dxSetter_ = nullptr;
	Input* input_ = nullptr;
	AudioManager* audioManager_ = nullptr;
	GlobalVariables* gv_ = nullptr;
	MapEditor* mapEditor_ = nullptr;

	std::unique_ptr<Camera> camera_;

	std::unique_ptr<FollowCamera> followCamera_;

	std::unique_ptr<Skybox> skybox_;

	std::unique_ptr<Model> skyDome_;
	std::unique_ptr<Transform> skyDomeTransform_;

	std::unique_ptr<Model> skyDomeNet_;
	std::unique_ptr<Transform> skyDomeNetTransform_;

	std::unique_ptr<Stage> stage_;

	std::unique_ptr<Sprite> pauseSprite_;
	std::unique_ptr<Sprite> menuSprite_;
	std::unique_ptr<Sprite> leftSprite_;
	std::unique_ptr<Sprite> rightSprite_;
	std::unique_ptr<Sprite> rbSprite_;
	std::unique_ptr<Sprite> toGameSprite_;
	std::unique_ptr<Sprite> toSelectSprite_;

	int32_t resetCount_ = 60;

	uint32_t testSE_;

	Texture* pauseTex_;
	Texture* menuTex_;
	Texture* leftTex_;
	Texture* rightTex_;
	Texture* rbTex_;
	Texture* toGameTex_;
	Texture* toSelectTex_;

	//ポーズ画面を開いているか
	bool isPause_ = false;

};


