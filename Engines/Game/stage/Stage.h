#pragma once
#include "Engine/3d/Model.h"
#include <memory>
#include "Game/player/Player.h"
#include <string>
#include "Game/block/Block.h"
#include "Game/goal/Goal.h"

class Stage
{
public:
	Stage();
	~Stage();

	void Initialize();

	void Update();

	void Draw(Camera* camera);

	void DrawParticle(Camera* camera);

	void LoadStage(const std::string& filename);

	const std::string& kDirectoryPath_ = "./resources/Maps/";

	bool GetAllBlockRock();

private:

	void CheckRockBlock(std::shared_ptr<Block> checkBlock, uint32_t num);

private:

	//マップに配置されているオブジェクトの構造体
	struct MapObject {

		std::string objName;

		std::unique_ptr<Model> model;

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

	std::list<std::shared_ptr<Goal>> goals_;

};

