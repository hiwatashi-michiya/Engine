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
	copyCats_.clear();
	enemies_.clear();
	holders_.clear();
	ColorHolder::ResetHolder();

	stageColor_ = GameColor::kWhite;
	stageParticle_->Initialize();
	stageParticle_->Load("./Resources/ParticleData/stage.json");
	stageParticle_->SetInstanceCount(128);
	stageParticle_->SetMinMaxSpawnPoint(player_->GetPosition() + Vector3{ -30.0f,-10.0f,-30.0f },
		player_->GetPosition() + Vector3{ 30.0f,-5.0f,30.0f });

}

void Stage::Update() {

#ifdef _DEBUG

	if (Input::GetInstance()->TriggerKey(DIK_H) and Input::GetInstance()->PushKey(DIK_LCONTROL)) {
		Line::showCollisionLine_ = not Line::showCollisionLine_;
	}

#endif // _DEBUG


	rings_.remove_if([](std::shared_ptr<Paint> ring) {

		if (ring->GetIsVanish()) {
			return true;
		}

		return false;

	});

	enemies_.remove_if([](std::shared_ptr<Enemy> enemy) {

		if (enemy->GetIsDead()) {
			return true;
		}

		return false;

		});

	//ゴール可能の状態でない時
	if (not player_->GetCanGoal()) {
		//プレイヤーがゴール可能か確認
		SetPlayerCanGoal();
	}

	if (followCamera_) {

		//カメラの切り替えが終わった瞬間且つ横から視点の場合、補正を掛ける
		if (followCamera_->GetIsSwitched() and followCamera_->GetCameraType() == CameraType::kSide) {
			player_->CorrectionPosition();
		}

		//カメラが動いている時プレイヤーの更新を止める
		if (not followCamera_->GetIsSwitching()) {
			player_->Update();
		}

	}
	else {
		player_->Update();
	}

	for (auto& block : blocks_) {

		block->Update();

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

	for (auto& enemy : enemies_) {
		enemy->Update();
	}

	for (auto& holder : holders_) {
		holder->Update();
	}

	for (auto& goal : goals_) {
		goal->Update();
	}

	stageParticle_->SetMinMaxSpawnPoint(player_->GetPosition() + Vector3{ -50.0f,-10.0f,-50.0f },
		player_->GetPosition() + Vector3{ 50.0f,-5.0f,50.0f });
	
	stageParticle_->SetEndColor(CreateColor(stageColor_));

	stageParticle_->Update();

}

void Stage::Draw(Camera* camera) {

	line_.origin = camera->GetWorldPosition();
	line_.diff = player_->GetPosition() - camera->GetWorldPosition();

	for (auto& ring : rings_) {

		if ((followCamera_->GetCameraType() == CameraType::kSide and
			ring->GetCollider()->collider_.center.z + ring->GetCollider()->collider_.size.z > player_->GetMinZ()) or
			(followCamera_->GetCameraType() == CameraType::kAbove and
				ring->GetCollider()->collider_.center.y - ring->GetCollider()->collider_.size.y < player_->GetMaxY())) {
			ring->Draw(camera);
		}

	}

	for (auto& block : blocks_) {

		if ((followCamera_->GetCameraType() == CameraType::kSide and
			block->GetCollider()->collider_.center.z + block->GetCollider()->collider_.size.z > player_->GetMinZ()) or
			(followCamera_->GetCameraType() == CameraType::kAbove and
				block->GetCollider()->collider_.center.y - block->GetCollider()->collider_.size.y < player_->GetMaxY())) {
			block->Draw(camera);
		}

	}

	for (auto& ghostBox : ghostBoxes_) {

		if ((followCamera_->GetCameraType() == CameraType::kSide and
			ghostBox->GetCollider()->collider_.center.z + ghostBox->GetCollider()->collider_.size.z > player_->GetMinZ()) or
			(followCamera_->GetCameraType() == CameraType::kAbove and
				ghostBox->GetCollider()->collider_.center.y - ghostBox->GetCollider()->collider_.size.y < player_->GetMaxY())) {
			ghostBox->Draw(camera);
		}
		
	}

	for (auto& warp : warps_) {
		warp->Draw(camera);
	}

	for (auto& colorSwitch : switches_) {
		colorSwitch->Draw(camera);
	}

	for (auto& enemy : enemies_) {
		enemy->Draw(camera);
	}

	for (auto& holder : holders_) {
		holder->Draw(camera);
	}

	for (auto& goal : goals_) {

		if ((followCamera_->GetCameraType() == CameraType::kSide and
			goal->GetCollider()->collider_.center.z + goal->GetCollider()->collider_.size.z > player_->GetMinZ()) or
			(followCamera_->GetCameraType() == CameraType::kAbove and
				goal->GetCollider()->collider_.center.y - goal->GetCollider()->collider_.size.y < player_->GetMaxY())) {
			goal->Draw(camera);
		}

	}

	player_->Draw(camera);

	stageParticle_->Draw(camera);

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

		if (object->tag == "item" || object->tag == Paint::objectName_) {
			std::shared_ptr<Paint> newObject = std::make_shared<Paint>();
			newObject->Initialize(object->transforms_[0]->translate_);
			newObject->SetColor(object->color);
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
			newObject->SetColor(object->color);
			moveBoxes_.push_back(newObject);
		}

		if (object->tag == "warp") {
			std::shared_ptr<Warp> newObject = std::make_shared<Warp>();
			newObject->Initialize();
			newObject->SetPosition(object->transforms_[0]->translate_);
			newObject->SetScale(object->transforms_[0]->scale_);
			newObject->SetPositionB(object->transforms_[1]->translate_);
			newObject->SetScaleB(object->transforms_[1]->scale_);
			newObject->SetColor(object->color);
			warps_.push_back(newObject);
		}

		if (object->tag == "ghostBox") {
			std::shared_ptr<GhostBox> newObject = std::make_shared<GhostBox>();
			newObject->Initialize();
			newObject->SetPosition(object->transforms_[0]->translate_);
			newObject->SetScale(object->transforms_[0]->scale_);
			newObject->SetColor(object->color);
			
			if (object->isRotateRight_) {
				newObject->SetRotateType(CommonVariables::RotateType::kClockwise);
			}
			else {
				newObject->SetRotateType(CommonVariables::RotateType::kCounterclockwise);
			}

			ghostBoxes_.push_back(newObject);
		}

		if (object->tag == "switch") {
			std::shared_ptr<Switch> newObject = std::make_shared<Switch>();
			newObject->Initialize();
			newObject->SetPosition(object->transforms_[0]->translate_);
			newObject->SetColor(object->color);
			switches_.push_back(newObject);
		}

		if (object->tag == "copyCat") {
			std::shared_ptr<CopyCat> newObject = std::make_shared<CopyCat>();
			newObject->Initialize();
			newObject->SetPosition(object->transforms_[0]->translate_);
			newObject->SetRespawnPosition(object->transforms_[0]->translate_);
			newObject->SetColor(object->color);
			newObject->SetPlayer(player_.get());
			copyCats_.push_back(newObject);
		}

		if (object->tag == "enemy") {
			std::shared_ptr<Enemy> newObject = std::make_shared<Enemy>();
			newObject->Initialize();
			newObject->SetPosition(object->transforms_[0]->translate_);
			newObject->SetColor(object->color);
			enemies_.push_back(newObject);
		}

		if (object->tag == "enemy") {
			std::shared_ptr<Enemy> newObject = std::make_shared<Enemy>();
			newObject->Initialize();
			newObject->SetPosition(object->transforms_[0]->translate_);
			newObject->SetColor(object->color);
			enemies_.push_back(newObject);
		}

		if (object->tag == "colorHolder") {
			std::shared_ptr<ColorHolder> newObject = std::make_shared<ColorHolder>();
			newObject->Initialize();
			newObject->SetPosition(object->transforms_[0]->translate_);
			holders_.push_back(newObject);
		}

	}

	//プレイヤーのゴールカウントを設定
	player_->SetGoalCount(int32_t(rings_.size()));

}

void Stage::SetPlayerCanGoal()
{

	for (auto& ghostBox : ghostBoxes_) {

		if (ghostBox->IsWhite()) {
			player_->SetCanGoal(false);
			return;
		}

	}

	player_->SetCanGoal(true);

}
