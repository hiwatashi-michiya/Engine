#pragma once
#include <string>
#include <vector>
#include <list>
#include <memory>
#include "Model.h"
#include "Transform.h"
#include "Game/ColorSetter/ColorSetter.h"
#include "GameObject.h"
#include "ColorCounter.h"
#include "Game/player/Player.h"

class StageLoader
{
public:
	StageLoader();
	~StageLoader();

	//ステージ読み込み
	void LoadStage(uint32_t stageNumber, std::vector<std::unique_ptr<GameObject>>& gameObjects);

	//読み込み先のディレクトリパス
	const std::string& kDirectoryPath_ = "./resources/Maps/";

private:

	//マップに配置されているオブジェクトの構造体
	struct MapObject {

		std::string objName;

		std::unique_ptr<Model> model;

		std::vector<std::unique_ptr<Transform>> transforms_;

		//メッシュ
		std::string meshName;

		int32_t meshNumber = 0;

		//オブジェクトの役割を表すタグ
		std::string tag;

		int32_t tagNumber = 0;

		bool isRotateRight_ = true;

		bool isSelect = false;

		GameColor::Color color = GameColor::kWhite;

	};

	//オブジェクトリスト
	std::list<std::unique_ptr<MapObject>> mapObjData_;

};


