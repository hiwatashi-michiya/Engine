#include "StageLoader.h"
#include "Externals/nlohmann/json.hpp"
#include <fstream>
#include "Game/block/Block.h"
#include "Game/Gimmick/GhostBox.h"
#include "Game/Gimmick/Switch.h"

StageLoader::StageLoader()
{
}

StageLoader::~StageLoader()
{
}

void StageLoader::LoadStage(uint32_t stageNumber, std::vector<std::unique_ptr<GameObject>>& gameObjects) {

	mapObjData_.clear();
	gameObjects.clear();

	std::string sceneName = "marScene";

	std::string stageName = "stage";

	stageName += std::to_string(stageNumber);

	//読み込むJSONファイルのフルパスを合成する
	std::string filePath = kDirectoryPath_ + stageName + ".json";
	//読み込み用ファイルストリーム
	std::ifstream ifs;
	//ファイルを読み込み用に開く
	ifs.open(filePath);

	//ファイルオープン失敗したら表示
	if (ifs.fail()) {
		MessageBox(nullptr, L"指定したファイルは存在しません。", L"Map Editor - Load", 0);
		return;
	}

	nlohmann::json root;

	//json文字列からjsonのデータ構造に展開
	ifs >> root;
	//ファイルを閉じる
	ifs.close();
	//グループを検索
	nlohmann::json::iterator itGroup = root.find(sceneName);
	//未登録チェック
	if (itGroup == root.end()) {
		MessageBox(nullptr, L"ファイルの構造が正しくありません。", L"Map Editor - Load", 0);
		return;
	}

	//保険
	assert(itGroup != root.end());

	//各アイテムについて
	for (nlohmann::json::iterator itItem = itGroup->begin(); itItem != itGroup->end(); ++itItem) {

		//アイテム名を取得
		const std::string& itemName = itItem.key();

		//グループを検索
		nlohmann::json::iterator itObject = itGroup->find(itemName);

		//未登録チェック
		if (itObject == itGroup->end()) {
			MessageBox(nullptr, L"ファイルの構造が正しくありません。", L"Map Editor - Load", 0);
		}

		//保険
		assert(itObject != itGroup->end());

		//アイテム名がオブジェクトデータだった場合、登録
		if (itemName == "objectData") {

			//各オブジェクトについて
			for (nlohmann::json::iterator itObjectData = itObject->begin(); itObjectData != itObject->end(); ++itObjectData) {

				//アイテム名を取得
				const std::string& objectName = itObjectData.key();

				//グループを検索
				nlohmann::json::iterator itData = itObject->find(objectName);

				//未登録チェック
				if (itData == itObject->end()) {
					MessageBox(nullptr, L"ファイルの構造が正しくありません。", L"Map Editor - Load", 0);
				}

				//保険
				assert(itData != itObject->end());

				std::unique_ptr<MapObject> mapObject = std::make_unique<MapObject>();

				mapObject->isSelect = true;
				mapObject->model = std::make_unique<Model>();
				mapObject->model->Initialize("./resources/block/block.obj");
				mapObject->transforms_.resize(2);
				mapObject->transforms_[0] = std::make_unique<Transform>();
				mapObject->objName = objectName;

				uint32_t roopCount = 0;

				for (nlohmann::json::iterator itItemObject = itData->begin(); itItemObject != itData->end(); ++itItemObject) {

					//アイテム名を取得
					const std::string& itemNameObject = itItemObject.key();

					//要素数3の配列であれば
					if (itItemObject->is_array() and itItemObject->size() == 3) {

						//名前がpositionだった場合、positionを登録
						if (itemNameObject == "position") {
							//float型のjson配列登録
							mapObject->transforms_[0]->translate_ = { itItemObject->at(0), itItemObject->at(1), itItemObject->at(2) };
						}
						//名前がrotationだった場合、rotationを登録
						else if (itemNameObject == "rotation") {
							//float型のjson配列登録
							mapObject->transforms_[0]->rotate_ = { itItemObject->at(0), itItemObject->at(1), itItemObject->at(2) };
						}
						//名前がscaleだった場合、scaleを登録
						else if (itemNameObject == "scale") {
							//float型のjson配列登録
							mapObject->transforms_[0]->scale_ = { itItemObject->at(0), itItemObject->at(1), itItemObject->at(2) };
						}

					}
					//Vector3以外の場合
					else {

						//名前がpositionだった場合、positionを登録
						if (itemNameObject == "position") {
							//float型のjson配列登録
							mapObject->transforms_[0]->translate_ = { itItemObject->at(0), itItemObject->at(1), itItemObject->at(2) };
							mapObject->transforms_[1] = std::make_unique<Transform>();
							mapObject->transforms_[1]->translate_ = { itItemObject->at(3), itItemObject->at(4), itItemObject->at(5) };
						}

						if (itemNameObject == "color") {
							mapObject->color = GameColor::Color(itItemObject->get<int32_t>());
						}

						if (itemNameObject == "RotateRight") {
							mapObject->isRotateRight_ = itItemObject->get<bool>();
						}

						//クォータニオン追加
						if (itemNameObject == "quaternion") {
							//float型のjson配列登録
							mapObject->transforms_[0]->rotateQuaternion_ = { itItemObject->at(0), itItemObject->at(1), itItemObject->at(2),itItemObject->at(3) };
						}

						//タグを登録
						if (itemNameObject == "tag") {
							mapObject->tag = itItemObject.value();
						}
						else if (itemNameObject == "tagNumber") {
							mapObject->tagNumber = itItemObject->get<int32_t>();
						}
						//メッシュを登録
						else if (itemNameObject == "mesh") {
							mapObject->meshName = itItemObject.value();
						}
						else if (itemNameObject == "meshNumber") {
							mapObject->meshNumber = itItemObject->get<int32_t>();
						}

					}

					roopCount++;

				}

				mapObjData_.push_back(std::move(mapObject));

			}

		}
		//カウント登録
		/*if (itemName == "Count") {

			counter->SetGoalCount(itItem->at(0), itItem->at(1), itItem->at(2));

		}*/

	}

	//マップオブジェクトのデータを基にオブジェクトを生成
	for (auto& object : mapObjData_) {

		/*if (object->tag == "player") {
			player.SetPosition(object->transforms_[0]->translate_);
		}*/

		if (object->tag == "block") {
			std::unique_ptr<Block> newObject = std::make_unique<Block>();
			newObject->Initialize();
			newObject->SetPosition(object->transforms_[0]->translate_);
			newObject->SetScale(object->transforms_[0]->scale_);
			gameObjects.push_back(std::move(newObject));
		}

		if (object->tag == "ghostBox") {
			std::unique_ptr<GhostBox> newObject = std::make_unique<GhostBox>();
			newObject->Initialize();
			newObject->SetPosition(object->transforms_[0]->translate_);
			newObject->SetScale(object->transforms_[0]->scale_);
			newObject->SetColor(object->color);

			gameObjects.push_back(std::move(newObject));
		}

		if (object->tag == "switch") {
			std::unique_ptr<Switch> newObject = std::make_unique<Switch>();
			newObject->Initialize();
			newObject->SetPosition(object->transforms_[0]->translate_);
			newObject->SetScale(object->transforms_[0]->scale_);
			newObject->SetColor(object->color);
			gameObjects.push_back(std::move(newObject));
		}

	}

}
