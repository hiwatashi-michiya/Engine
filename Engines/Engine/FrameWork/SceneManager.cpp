#include "SceneManager.h"
#include <cassert>
#include "Core/DirectXSetter.h"
#include "Model.h"
#include "Particle3D.h"
#include "Sprite.h"
#include "PostEffectDrawer.h"
#include "Skinning/SkinningModel.h"
#include "LineDrawer.h"
#include "Drawing/RenderManager.h"

SceneManager* SceneManager::GetInstance() {
	static SceneManager instance;
	return &instance;
}

void SceneManager::Update() {

	if (nextScene_) {

		//旧シーンの終了
		if (scene_) {
			scene_->Finalize();
			scene_.reset();
		}

		//シーン切り替え
		scene_.reset(nextScene_.release());

		scene_->SetSceneManager(this);

		scene_->Initialize();

	}

	scene_->Update();

}

void SceneManager::Finalize()
{

	scene_->Finalize();

}

void SceneManager::Draw() {

	scene_->Draw();

}

void SceneManager::Render()
{

	RenderManager::GetInstance()->Render();

}

void SceneManager::ChangeScene(const std::string& sceneName) {

	assert(sceneFactory_);

	assert(nextScene_ == nullptr);

	nextScene_.reset(sceneFactory_->CreateScene(sceneName));

}
