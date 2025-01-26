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
#include <cmath>
#include <windows.h>
#include <process.h>
#include <mmsystem.h>

class Item
{
public:

	void Initialize();

	void Update();

	void Draw();

	void SetIsActive(bool flag) { isActive_ = flag; }

	void SetPosition(const Vector2& position) { position_ = position; }

	bool GetIsActive() const { return isActive_; }

	bool CheckCollision(const Vector2& position, const Vector2& size);

private:

	std::unique_ptr<Sprite> item_;
	Texture* tex_ = nullptr;

	Vector2 position_{};
	bool isActive_ = true;

};

struct NT_Player {
	Vector2 position{};
	int32_t count = 0;
};

/// <summary>
/// 新機能を試すためのシーン
/// </summary>
class TestScene : public DefaultScene
{
public:

	static const uint32_t kMaxItem_ = 49;

	TestScene();
	~TestScene() override;

	void Initialize() override;

	void Finalize() override;

	void Update() override;

	void Draw() override;

private:

	std::unique_ptr<Sprite> sPlayerSprite_;
	std::unique_ptr<Sprite> cPlayerSprite_;
	std::unique_ptr<Sprite> winSprite_;
	std::unique_ptr<Sprite> loseSprite_;
	std::unique_ptr<Sprite> bgSprite_;
	std::array<std::unique_ptr<Sprite>, 2> sCountSprite_;
	std::array<std::unique_ptr<Sprite>, 2> cCountSprite_;

	static std::array<std::unique_ptr<Item>, kMaxItem_> items_;

	Texture* playerTex_ = nullptr;
	Texture* numTex_ = nullptr;
	Texture* winTex_ = nullptr;
	Texture* loseTex_ = nullptr;
	Texture* bgTex_ = nullptr;

	static NT_Player sPlayer_, cPlayer_;
	//サーバー側関数
	static DWORD WINAPI SThreadfunc(void*);
	//クライアント側関数
	static DWORD WINAPI CThreadfunc(void*);
	//サーバー側かどうか
	bool isServer_ = true;
	//試合が終わったかどうか
	bool isEnd_ = false;
	//勝ったかどうか
	bool isWin_ = false;
	//すべて取ったかどうか
	bool isGetAll_ = false;

};
