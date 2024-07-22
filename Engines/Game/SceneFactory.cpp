#include "SceneFactory.h"
#include "GameScene/GameScene.h"
#include "TitleScene/TitleScene.h"
#include "SelectScene/SelectScene.h"
#include "EditorScene/EditorScene.h"

BaseScene* SceneFactory::CreateScene(const std::string& sceneName) {

	BaseScene* newScene = nullptr;

	if (sceneName == "TITLE") {
		newScene = new TitleScene();
	}
	else if (sceneName == "GAMEPLAY") {
		newScene = new GameScene();
	}
	else if (sceneName == "SELECT") {
		newScene = new SelectScene();
	}
	else if (sceneName == "EDITOR") {
		newScene = new EditorScene();
	}

	return newScene;

}
