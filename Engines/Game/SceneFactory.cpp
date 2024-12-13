#include "SceneFactory.h"
#include "GameScene/GameScene.h"
#include "TitleScene/TitleScene.h"
#include "SelectScene/SelectScene.h"
#include "EditorScene/EditorScene.h"
#include "TestScene/TestScene.h"

BaseScene* SceneFactory::CreateScene(const std::string& sceneName) {
	//新しいポインタ作成
	BaseScene* newScene = nullptr;
	//文字列に応じてシーン切り替え
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
	else if (sceneName == "TEST") {
		newScene = new TestScene();
	}

	return newScene;

}
