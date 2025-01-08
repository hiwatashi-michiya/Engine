#include "Stage.h"
#include "Collision.h"
#include "Externals/nlohmann/json.hpp"
#include <fstream>
#include "Audio/AudioManager.h"
#include "ImGuiManager.h"
#include "PostEffectDrawer.h"
#include "UsefulFunc.h"
#include "Game/Variables/CommonVariables.h"

using namespace CommonVariables;

GameColor::Color Stage::stageColor_ = GameColor::kWhite;

Stage::Stage()
{
	player_ = std::make_unique<Player>();
	stageParticle_ = std::make_unique<Particle>();
	counter_ = std::make_unique<ColorCounter>();
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
	gameObjects_.clear();
	ColorHolder::ResetHolder();
	counter_->Initialize();

	stageColor_ = GameColor::kWhite;
	stageParticle_->Initialize();
	stageParticle_->Load("./Resources/ParticleData/stage.json");
	stageParticle_->SetInstanceCount(128);
	stageParticle_->SetMinMaxSpawnPoint(player_->GetPosition() + minParticleLange_,
		player_->GetPosition() + maxParticleLange_);

}

void Stage::Update() {

#ifdef _DEBUG

	if (Input::GetInstance()->TriggerKey(DIK_H) and Input::GetInstance()->PushKey(DIK_LCONTROL)) {
		Line::showCollisionLine_ = not Line::showCollisionLine_;
	}

#endif // _DEBUG

	//フラグが立ったものを削除
	rings_.remove_if([](const std::unique_ptr<Paint>& ring) {

		if (ring->GetIsVanish()) {
			return true;
		}

		return false;

	});

	//ゴール可能の状態でない時
	if (not player_->GetCanGoal()) {
		//プレイヤーがゴール可能か確認
		SetPlayerCanGoal();
	}

	player_->Update();

	//各オブジェクト更新
	for (const std::unique_ptr<Block>& block : blocks_) {

		block->Update();

	}

	for (const std::unique_ptr<GhostBox>& ghostBox : ghostBoxes_) {
		ghostBox->Update();
	}

	for (const std::unique_ptr<Paint>& ring : rings_) {
		ring->Update();
	}

	for (const std::unique_ptr<GameObject>& gameObject : gameObjects_) {
		gameObject;
	}

	for (const std::unique_ptr<Switch>& colorSwitch : switches_) {
		colorSwitch->Update();
	}

	for (const std::unique_ptr<Goal>& goal : goals_) {
		goal->Update();
	}

	counter_->Update();

	//パーティクルのスポーン地点更新
	stageParticle_->SetMinMaxSpawnPoint(player_->GetPosition() + minParticleLange_,
		player_->GetPosition() + maxParticleLange_);
	
	stageParticle_->SetEndColor(CreateColor(stageColor_));

	stageParticle_->Update();

}

void Stage::Draw(Camera* camera) {

	line_.origin = camera->GetWorldPosition();
	line_.diff = player_->GetPosition() - camera->GetWorldPosition();

	for (const std::unique_ptr<Paint>& ring : rings_) {
		ring->Draw(camera);
	}

	for (const std::unique_ptr<Block>& block : blocks_) {
		block->Draw(camera);
	}

	for (const std::unique_ptr<GhostBox>& ghostBox : ghostBoxes_) {
		ghostBox->Draw(camera);
	}

	for (const std::unique_ptr<Switch>& colorSwitch : switches_) {
		colorSwitch->Draw(camera);
	}

	for (const std::unique_ptr<Goal>& goal : goals_) {
		goal->Draw(camera);
	}

	player_->Draw(camera);

	stageParticle_->Draw(camera);

	counter_->Draw();

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

				std::unique_ptr<MapObject> mapObject = std::make_unique<MapObject>();

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
					if (itItemObject->is_array() and itItemObject->size() == 3) {

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
							mapObject->color = GameColor::Color(itItemObject->get<int32_t>());
						}

						if (itemNameObject == "RotateRight") {
							mapObject->isRotateRight_ = itItemObject->get<bool>();
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

				mapObjData_.push_back(std::move(mapObject));

			}

		}
		//カウント登録
		if (itemName == "Count") {

			counter_->SetGoalCount(itItem->at(0), itItem->at(1), itItem->at(2));

		}

	}

	//マップオブジェクトのデータを基にオブジェクトを生成
	for (auto& object : mapObjData_) {

		if (object->tag == "player") {
			player_->SetPosition(object->transforms_[0]->translate_);
		}

		if (object->tag == "block") {
			std::unique_ptr<Block> newObject = std::make_unique<Block>();
			newObject->Initialize();
			newObject->SetPosition(object->transforms_[0]->translate_);
			newObject->SetScale(object->transforms_[0]->scale_);
			blocks_.push_back(std::move(newObject));
		}

		if (object->tag == "item" || object->tag == Paint::objectName_) {
			std::unique_ptr<Paint> newObject = std::make_unique<Paint>();
			newObject->Initialize(object->transforms_[0]->translate_);
			newObject->SetColor(object->color);
			rings_.push_back(std::move(newObject));
		}

		if (object->tag == "goal") {
			std::unique_ptr<Goal> newObject = std::make_unique<Goal>();
			newObject->Initialize();
			newObject->SetPosition(object->transforms_[0]->translate_);
			newObject->SetPlayer(player_.get());
			goals_.push_back(std::move(newObject));
		}

		if (object->tag == "moveBox") {
			std::unique_ptr<MoveBox> newObject = std::make_unique<MoveBox>();
			newObject->Initialize();
			newObject->SetPosition(object->transforms_[0]->translate_);
			newObject->SetScale(object->transforms_[0]->scale_);
			newObject->SetColor(object->color);
			moveBoxes_.push_back(std::move(newObject));
		}

		if (object->tag == "warp") {
			std::unique_ptr<Warp> newObject = std::make_unique<Warp>();
			newObject->Initialize();
			newObject->SetPosition(object->transforms_[0]->translate_);
			newObject->SetScale(object->transforms_[0]->scale_);
			newObject->SetPositionB(object->transforms_[1]->translate_);
			newObject->SetScaleB(object->transforms_[1]->scale_);
			newObject->SetColor(object->color);
			gameObjects_.push_back(std::move(newObject));
		}

		if (object->tag == "ghostBox") {
			std::unique_ptr<GhostBox> newObject = std::make_unique<GhostBox>();
			newObject->Initialize();
			newObject->SetPosition(object->transforms_[0]->translate_);
			newObject->SetScale(object->transforms_[0]->scale_);
			newObject->SetColor(object->color);

			ghostBoxes_.push_back(std::move(newObject));
		}

		if (object->tag == "switch") {
			std::unique_ptr<Switch> newObject = std::make_unique<Switch>();
			newObject->Initialize();
			newObject->SetPosition(object->transforms_[0]->translate_);
			newObject->SetColor(object->color);
			switches_.push_back(std::move(newObject));
		}

		if (object->tag == "copyCat") {
			std::unique_ptr<CopyCat> newObject = std::make_unique<CopyCat>();
			newObject->Initialize();
			newObject->SetPosition(object->transforms_[0]->translate_);
			newObject->SetRespawnPosition(object->transforms_[0]->translate_);
			newObject->SetColor(object->color);
			newObject->SetPlayer(player_.get());
			gameObjects_.push_back(std::move(newObject));
		}

		if (object->tag == "enemy") {
			std::unique_ptr<Enemy> newObject = std::make_unique<Enemy>();
			newObject->Initialize();
			newObject->SetPosition(object->transforms_[0]->translate_);
			newObject->SetColor(object->color);
			gameObjects_.push_back(std::move(newObject));
		}

		if (object->tag == "colorHolder") {
			std::unique_ptr<ColorHolder> newObject = std::make_unique<ColorHolder>();
			newObject->Initialize();
			newObject->SetPosition(object->transforms_[0]->translate_);
			gameObjects_.push_back(std::move(newObject));
		}

	}

	//プレイヤーのゴールカウントを設定
	player_->SetGoalCount(int32_t(rings_.size()));

}

void Stage::SetPlayerCanGoal()
{

	//1つでも白かったらゴールフラグを立たせない
	for (auto& ghostBox : ghostBoxes_) {

		if (ghostBox->IsWhite()) {
			player_->SetCanGoal(false);
			return;
		}

	}
	//1つでも白かったらゴールフラグを立たせない
	for (auto& block : blocks_) {

		if (block->IsWhite()) {
			player_->SetCanGoal(false);
			return;
		}

	}

	player_->SetCanGoal(true);

}
