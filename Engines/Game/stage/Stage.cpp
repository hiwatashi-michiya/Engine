#include "Stage.h"
#include "Collision.h"
#include "Externals/nlohmann/json.hpp"
#include <fstream>
#include "Audio/AudioManager.h"
#include "ImGuiManager.h"
#include "PostEffectDrawer.h"

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
	warps_.clear();
	GhostBox::SetLine(&line_);

}

void Stage::Update() {

#ifdef _DEBUG

	/*ImGui::Begin("origin");
	ImGui::DragFloat3("translate", &line_.origin.x, 0.1f);
	ImGui::End();*/

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

	for (auto& goal : goals_) {
		goal->Draw(camera);
	}

}

void Stage::DrawSkinningModel(Camera* camera) {

	player_->Draw(camera);

}

void Stage::DrawParticle(Camera* camera) {

	player_->DrawParticle(camera);

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
			std::shared_ptr<Block> newBlock = std::make_shared<Block>();
			newBlock->Initialize();
			newBlock->SetPosition(object->transforms_[0]->translate_);
			newBlock->SetScale(object->transforms_[0]->scale_);
			blocks_.push_back(newBlock);
		}

		if (object->tag == "item" || object->tag == "ring") {
			std::shared_ptr<Ring> newRing = std::make_shared<Ring>();
			newRing->Initialize(object->transforms_[0]->translate_);
			rings_.push_back(newRing);
		}

		if (object->tag == "goal") {
			std::shared_ptr<Goal> newGoal = std::make_shared<Goal>();
			newGoal->Initialize();
			newGoal->SetPosition(object->transforms_[0]->translate_);
			goals_.push_back(newGoal);
		}

		if (object->tag == "moveBox") {
			std::shared_ptr<MoveBox> newBox = std::make_shared<MoveBox>();
			newBox->Initialize();
			newBox->SetPosition(object->transforms_[0]->translate_);
			newBox->SetScale(object->transforms_[0]->scale_);
			moveBoxes_.push_back(newBox);
		}

		if (object->tag == "warp") {
			std::shared_ptr<Warp> newWarp = std::make_shared<Warp>();
			newWarp->Initialize();
			newWarp->SetPosition(object->transforms_[0]->translate_);
			newWarp->SetScale(object->transforms_[0]->scale_);
			newWarp->SetPositionB(object->transforms_[1]->translate_);
			newWarp->SetScaleB(object->transforms_[1]->scale_);
			warps_.push_back(newWarp);
		}

		if (object->tag == "ghostBox") {
			std::shared_ptr<GhostBox> newBox = std::make_shared<GhostBox>();
			newBox->Initialize();
			newBox->SetPosition(object->transforms_[0]->translate_);
			newBox->SetScale(object->transforms_[0]->scale_);
			ghostBoxes_.push_back(newBox);
		}

	}

}
