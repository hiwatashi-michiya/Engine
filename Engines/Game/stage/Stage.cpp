#include "Stage.h"
#include "Collision.h"
#include "Externals/nlohmann/json.hpp"
#include <fstream>
#include "Audio/AudioManager.h"
#include "ImGuiManager.h"
#include "PostEffectDrawer.h"

int32_t Stage::stageColor_ = 0;

Stage::Stage()
{
	player_ = std::make_unique<Player>();
}

Stage::~Stage()
{
}

void Stage::Initialize() {

	player_->Initialize();
	blocks_.clear();
	rings_.clear();
	mapObjData_.clear();
	goals_.clear();
	moveBoxes_.clear();
	ghostBoxes_.clear();
	switches_.clear();
	warps_.clear();

	stageColor_ = 0;

}

void Stage::Update() {

#ifdef _DEBUG

	

#endif // _DEBUG


	rings_.remove_if([](std::shared_ptr<Ring> ring) {

		if (ring->GetIsVanish()) {
			return true;
		}

		return false;

	});

	player_->Update();

	for (uint32_t i = 0; auto& block : blocks_) {

		block->Update();

		i++;

	}

	for (auto& box : moveBoxes_) {

		box->Update();

	}

	for (auto& ghostBox : ghostBoxes_) {
		ghostBox->Update();
	}

	for (auto& ring : rings_) {

		ring->Update();

	}

	for (auto& warp : warps_) {
		warp->Update();
	}

	for (auto& colorSwitch : switches_) {
		colorSwitch->Update();
	}

	for (auto& goal : goals_) {
		goal->Update();
	}

}

void Stage::Draw(Camera* camera) {

	line_.origin = camera->GetWorldPosition();
	line_.diff = player_->GetPosition() - camera->GetWorldPosition();

	for (auto& ring : rings_) {
		ring->Draw(camera);
	}

	for (auto& box : moveBoxes_) {

		box->Draw(camera);

	}

	for (auto& block : blocks_) {

		block->Draw(camera);

	}

	for (auto& ghostBox : ghostBoxes_) {
		ghostBox->Draw(camera);
	}

	for (auto& warp : warps_) {
		warp->Draw(camera);
	}

	for (auto& colorSwitch : switches_) {
		colorSwitch->Draw(camera);
	}

	for (auto& goal : goals_) {
		goal->Draw(camera);
	}

}

void Stage::DrawSkinningModel(Camera* camera) {

	player_->Draw(camera);

}

void Stage::DrawParticle(Camera* camera) {

	player_->DrawParticle(camera);

	for (auto& goal : goals_) {
		goal->DrawParticle(camera);
	}

}

void Stage::DrawLine(Camera* camera) {

	player_->DrawLine(camera);

	for (auto& block : blocks_) {

		block->DrawLine(camera);
	}

	for (auto& box : moveBoxes_) {

		box->DrawLine(camera);
	}

	for (auto& ring : rings_) {

		ring->DrawLine(camera);
	}

	for (auto& warp : warps_) {
		warp->DrawLine(camera);
	}

	for (auto& goal : goals_) {
		goal->DrawLine(camera);
	}

	for (auto& colorSwitch : switches_) {
		colorSwitch->DrawLine(camera);
	}

	for (auto& ghostBox : ghostBoxes_) {
		ghostBox->DrawLine(camera);
	}

}

void Stage::LoadStage(uint32_t stageNumber) {

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

				std::shared_ptr<MapObject> mapObject = std::make_shared<MapObject>();

				mapObject->isSelect = true;
				mapObject->model.reset(Model::Create("./resources/cube/cube.obj"));
				mapObject->transforms_.resize(2);
				mapObject->transforms_[0] = std::make_unique<Transform>();
				mapObject->objName = objectName;

				uint32_t roopCount = 0;

				for (nlohmann::json::iterator itItemObject = itData->begin(); itItemObject != itData->end(); ++itItemObject) {

					//アイテム名を取得
					const std::string& itemNameObject = itItemObject.key();

					//要素数3の配列であれば
					if (itItemObject->is_array() && itItemObject->size() == 3) {

						//名前がpositionだった場合、positionを登録
						if (itemNameObject == "position") {
							//float型のjson配列登録
							mapObject->transforms_[0]->translate_ = {itItemObject->at(0), itItemObject->at(1), itItemObject->at(2)};
						}
						//名前がrotationだった場合、rotationを登録
						else if (itemNameObject == "rotation") {
							//float型のjson配列登録
							mapObject->transforms_[0]->rotate_ = {itItemObject->at(0), itItemObject->at(1), itItemObject->at(2)};
						}
						//名前がscaleだった場合、scaleを登録
						else if (itemNameObject == "scale") {
							//float型のjson配列登録
							mapObject->transforms_[0]->scale_ = {itItemObject->at(0), itItemObject->at(1), itItemObject->at(2)};
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
							mapObject->colorNumber = itItemObject->get<int32_t>();
						}

						//クォータニオン追加
						if (itemNameObject == "quaternion") {
							//float型のjson配列登録
							mapObject->transforms_[0]->rotateQuaternion_ = {itItemObject->at(0), itItemObject->at(1), itItemObject->at(2),itItemObject->at(3)};
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

				mapObjData_.push_back(mapObject);

			}

		}
	}

	//マップオブジェクトのデータを基にオブジェクトを生成
	for (auto& object : mapObjData_) {

		if (object->tag == "player") {
			player_->SetPosition(object->transforms_[0]->translate_);
		}

		if (object->tag == "block") {
			std::shared_ptr<Block> newObject = std::make_shared<Block>();
			newObject->Initialize();
			newObject->SetPosition(object->transforms_[0]->translate_);
			newObject->SetScale(object->transforms_[0]->scale_);
			blocks_.push_back(newObject);
		}

		if (object->tag == "item" || object->tag == "ring") {
			std::shared_ptr<Ring> newObject = std::make_shared<Ring>();
			newObject->Initialize(object->transforms_[0]->translate_);
			newObject->SetColor(object->colorNumber);
			rings_.push_back(newObject);
		}

		if (object->tag == "goal") {
			std::shared_ptr<Goal> newObject = std::make_shared<Goal>();
			newObject->Initialize();
			newObject->SetPosition(object->transforms_[0]->translate_);
			newObject->SetPlayer(player_.get());
			goals_.push_back(newObject);
		}

		if (object->tag == "moveBox") {
			std::shared_ptr<MoveBox> newObject = std::make_shared<MoveBox>();
			newObject->Initialize();
			newObject->SetPosition(object->transforms_[0]->translate_);
			newObject->SetScale(object->transforms_[0]->scale_);
			newObject->SetColor(object->colorNumber);
			moveBoxes_.push_back(newObject);
		}

		if (object->tag == "warp") {
			std::shared_ptr<Warp> newObject = std::make_shared<Warp>();
			newObject->Initialize();
			newObject->SetPosition(object->transforms_[0]->translate_);
			newObject->SetScale(object->transforms_[0]->scale_);
			newObject->SetPositionB(object->transforms_[1]->translate_);
			newObject->SetScaleB(object->transforms_[1]->scale_);
			newObject->SetColor(object->colorNumber);
			warps_.push_back(newObject);
		}

		if (object->tag == "ghostBox") {
			std::shared_ptr<GhostBox> newObject = std::make_shared<GhostBox>();
			newObject->Initialize();
			newObject->SetPosition(object->transforms_[0]->translate_);
			newObject->SetScale(object->transforms_[0]->scale_);
			newObject->SetColor(object->colorNumber);
			ghostBoxes_.push_back(newObject);
		}

		if (object->tag == "switch") {
			std::shared_ptr<Switch> newObject = std::make_shared<Switch>();
			newObject->Initialize();
			newObject->SetPosition(object->transforms_[0]->translate_);
			newObject->SetColor(object->colorNumber);
			switches_.push_back(newObject);
		}

	}

	//プレイヤーのゴールカウントを設定
	player_->SetGoalCount(int32_t(rings_.size()));

}
