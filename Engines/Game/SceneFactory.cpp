#include "SceneFactory.h"
#include "gamescene/GameScene.h"
#include "TitleScene/TitleScene.h"
#include "EditorScene/EditorScene.h"

BaseScene* SceneFactory::CreateScene(const std::string& sceneName) {

	BaseScene* newScene = nullptr;

	if (sceneName == "TITLE") {
		newScene = new TitleScene();
	}
	else if (sceneName == "GAMEPLAY") {
		newScene = new GameScene();
	}
	else if (sceneName == "EDIT") {
		newScene = new EditorScene();
	}

	return newScene;

}
