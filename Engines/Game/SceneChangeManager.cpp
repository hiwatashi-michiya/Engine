#include "SceneChangeManager.h"
#include "SceneManager.h"
#include "Core/DirectXSetter.h"

SceneChangeManager* SceneChangeManager::GetInstance() {
	static SceneChangeManager instance;
	return &instance;
}

void SceneChangeManager::Initialize() {

	texture_ = TextureManager::GetInstance()->Load("./Resources/white.png");
	sprite_.reset(Sprite::Create(texture_, {}, { 0.0f,0.0f,0.0f,1.0f }));
	sprite_->size_ = { 1280.0f,720.0f };

}

void SceneChangeManager::Update() {

	if (isSceneChangeEnter_) {

		sceneChangeValue_ += sceneChangeSpeed_;

		if (sceneChangeValue_ >= 1.0f) {
			sceneChangeValue_ = 1.0f;
			SceneManager::GetInstance()->ChangeScene(nextScene_);
			isSceneChangeExit_ = true;
			isSceneChangeEnter_ = false;
		}

	}
	else if (isSceneChangeExit_) {

		sceneChangeValue_ -= sceneChangeSpeed_;

		if (sceneChangeValue_ <= 0.0f) {
			sceneChangeValue_ = 0.0f;
			isSceneChangeExit_ = false;
			isSceneChangeEnter_ = false;
		}

	}

	sprite_->color_.w = sceneChangeValue_;

}

void SceneChangeManager::Draw() {

	sprite_->Draw();

}

void SceneChangeManager::SceneChangeStart() {

	sceneChangeValue_ = 0.0f;
	isSceneChangeEnter_ = true;

}
