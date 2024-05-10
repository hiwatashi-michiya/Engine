#pragma once
#include "Engine/3d/Model.h"
#include <memory>
#include "Game/Player/Player.h"
#include <string>
#include "Game/Block/Block.h"
#include "Game/Item/Ring.h"

class Stage
{
public:
	Stage();
	~Stage();

	void Initialize();

	void Update();

	void Draw(Camera* camera);

	void DrawSkinningModel(Camera* camera);

	void DrawParticle(Camera* camera);

	void DrawLine(Camera* camera);

	void LoadStage(uint32_t stageNumber);

	const std::string& kDirectoryPath_ = "./resources/Maps/";

	Player* GetPlayer() const { return player_.get(); }

private:



private:

	//マップに配置されているオブジェクトの構造体
	struct MapObject {

		std::string objName;

		std::unique_ptr<Model> model;

		std::unique_ptr<Transform> transform;

		//メッシュ
		std::string meshName;

		int32_t meshNumber = 0;

		//オブジェクトの役割を表すタグ
		std::string tag;

		int32_t tagNumber = 0;

		bool isSelect = false;

	};

	std::unique_ptr<Player> player_;

	std::list<std::shared_ptr<MapObject>> mapObjData_;

	std::list<std::shared_ptr<Block>> blocks_;

	std::list<std::shared_ptr<Ring>> rings_;

};

