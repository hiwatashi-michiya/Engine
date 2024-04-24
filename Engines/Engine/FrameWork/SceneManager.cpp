#include "SceneManager.h"
#include <cassert>
#include "base/DirectXSetter.h"
#include "Model.h"
#include "Particle3D.h"
#include "Sprite.h"

SceneManager* SceneManager::GetInstance() {
	static SceneManager instance;
	return &instance;
}

void SceneManager::Update() {

	if (nextScene_) {

		//旧シーンの終了
		if (scene_) {
			scene_.reset();
		}

		//シーン切り替え
		scene_.reset(nextScene_.release());

		scene_->SetSceneManager(this);

		scene_->Initialize();

	}

	scene_->Update();

}

void SceneManager::Draw() {

	Model::PreDraw(DirectXSetter::GetInstance()->GetCommandList());

	scene_->DrawModel();

	Model::PostDraw();

	Particle3D::PreDraw(DirectXSetter::GetInstance()->GetCommandList());

	scene_->DrawParticle();

	Particle3D::PostDraw();

	Sprite::PreDraw(DirectXSetter::GetInstance()->GetCommandList());

	scene_->DrawSprite();

	Sprite::PostDraw();

}

void SceneManager::ChangeScene(const std::string& sceneName) {

	assert(sceneFactory_);

	assert(nextScene_ == nullptr);

	nextScene_.reset(sceneFactory_->CreateScene(sceneName));

}
