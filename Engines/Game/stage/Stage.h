#pragma once
#include "Drawing/LineDrawer.h"
#include "Engine/3d/Model.h"
#include "Game/Block/Block.h"
#include "Game/Item/Ring.h"
#include "Game/Player/Player.h"
#include <memory>
#include <string>
#include "Game/Goal/Goal.h"
#include "Game/Gimmick/MoveBox.h"
#include "Game/Gimmick/Warp.h"
#include "Game/Gimmick/GhostBox.h"
#include "Game/Gimmick/Switch.h"
#include "Game/Gimmick/CopyCat.h"
#include "Game/Enemy/Enemy.h"

class Stage
{
public:
	Stage();
	~Stage();

	static int32_t stageColor_;

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

		std::vector<std::unique_ptr<Transform>> transforms_;

		//メッシュ
		std::string meshName;

		int32_t meshNumber = 0;

		//オブジェクトの役割を表すタグ
		std::string tag;

		int32_t tagNumber = 0;

		bool isSelect = false;

		int32_t colorNumber = 0;

	};

	std::unique_ptr<Player> player_;

	std::list<std::shared_ptr<MapObject>> mapObjData_;

	std::list<std::shared_ptr<Block>> blocks_;

	std::list<std::shared_ptr<Ring>> rings_;

	std::list<std::shared_ptr<Goal>> goals_;

	std::list<std::shared_ptr<MoveBox>> moveBoxes_;

	std::list<std::shared_ptr<Warp>> warps_;

	std::list<std::shared_ptr<GhostBox>> ghostBoxes_;

	std::list<std::shared_ptr<Switch>> switches_;

	std::list<std::shared_ptr<CopyCat>> copyCats_;

	std::list<std::shared_ptr<Enemy>> enemies_;

	Segment line_{};

};

