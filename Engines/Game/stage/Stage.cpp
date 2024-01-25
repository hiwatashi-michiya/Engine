#include "Stage.h"
#include "Engine/math/Collision.h"
#include "Externals/nlohmann/json.hpp"
#include <fstream>

Stage::Stage()
{
}

Stage::~Stage()
{
}

void Stage::Initialize() {

	player_ = std::make_unique<Player>();
	player_->Initialize();
	blocks_.clear();
	goals_.clear();
	mapObjData_.clear();

}

void Stage::Update() {

	player_->Update();

	for (auto& goal : goals_) {
		goal->Update();
	}

	for (uint32_t i = 0; auto & block : blocks_) {

		block->Update();

		if (IsCollision(block->GetCollision(), player_->GetCollision()) && 
			!block->GetIsRock()) {

			if (player_->move_ == Player::kUp) {
				block->SetPosition(block->GetPosition() + Vector3{ 0.0f,0.0f,2.0f });
				CheckRockBlock(block, i);
			}
			if (player_->move_ == Player::kDown) {
				block->SetPosition(block->GetPosition() + Vector3{ 0.0f,0.0f,-2.0f });
				CheckRockBlock(block, i);
			}
			if (player_->move_ == Player::kRight) {
				block->SetPosition(block->GetPosition() + Vector3{ 2.0f,0.0f,0.0f });
				CheckRockBlock(block, i);
			}
			if (player_->move_ == Player::kLeft) {
				block->SetPosition(block->GetPosition() + Vector3{ -2.0f,0.0f,0.0f });
				CheckRockBlock(block, i);
			}

		}

		if (IsCollision(block->GetCollision(), player_->GetCollision()) &&
			block->GetIsRock()) {

			if (player_->move_ == Player::kUp) {
				player_->SetPosition(block->GetPosition() + Vector3{ 0.0f,0.0f,-2.0f });
			}
			if (player_->move_ == Player::kDown) {
				player_->SetPosition(block->GetPosition() + Vector3{ 0.0f,0.0f,2.0f });
			}
			if (player_->move_ == Player::kRight) {
				player_->SetPosition(block->GetPosition() + Vector3{ -2.0f,0.0f,0.0f });
			}
			if (player_->move_ == Player::kLeft) {
				player_->SetPosition(block->GetPosition() + Vector3{ 2.0f,0.0f,0.0f });
			}

		}

		for (auto& goal : goals_) {
			
			if (!block->GetIsRock()) {

				if (IsCollision(block->GetCollision(), goal->GetCollision()) &&
					block->GetColor() == goal->GetColor()) {

					block->SetIsRock(true);
					goal->SetIsGoal(true);

				}

			}

		}

		i++;

	}

}

void Stage::Draw(Camera* camera) {

	

	for (auto& block : blocks_) {
		block->Draw(camera);
	}

	for (auto& goal : goals_) {
		goal->Draw(camera);
	}

	player_->Draw(camera);

}

void Stage::DrawParticle(Camera* camera) {

	for (auto& goal : goals_) {
		goal->DrawParticle(camera);
	}

	player_->DrawParticle(camera);

}

bool Stage::GetAllBlockRock() {

	for (auto& block : blocks_) {

		if (!block->GetIsRock()) {
			return false;
		}

	}

	return true;

}

void Stage::CheckRockBlock(std::shared_ptr<Block> checkBlock, uint32_t num) {

	for (uint32_t i = 0; auto & block : blocks_) {

		if (IsCollision(block->GetCollision(), checkBlock->GetCollision()) &&
			i != num) {

			//押し戻し
			if (player_->move_ == Player::kUp) {
				checkBlock->SetPosition(checkBlock->GetPosition() + Vector3{ 0.0f,0.0f,-2.0f });
				player_->SetPosition(checkBlock->GetPosition() + Vector3{ 0.0f,0.0f,-2.0f });
			}
			if (player_->move_ == Player::kDown) {
				checkBlock->SetPosition(checkBlock->GetPosition() + Vector3{ 0.0f,0.0f,2.0f });
				player_->SetPosition(checkBlock->GetPosition() + Vector3{ 0.0f,0.0f,2.0f });
			}
			if (player_->move_ == Player::kRight) {
				checkBlock->SetPosition(checkBlock->GetPosition() + Vector3{ -2.0f,0.0f,0.0f });
				player_->SetPosition(checkBlock->GetPosition() + Vector3{ -2.0f,0.0f,0.0f });
			}
			if (player_->move_ == Player::kLeft) {
				checkBlock->SetPosition(checkBlock->GetPosition() + Vector3{ 2.0f,0.0f,0.0f });
				player_->SetPosition(checkBlock->GetPosition() + Vector3{ 2.0f,0.0f,0.0f });
			}

		}

		i++;

	}

}

void Stage::LoadStage(const std::string& filename) {

	//読み込むJSONファイルのフルパスを合成する
	std::string filePath = kDirectoryPath_ + filename + ".json";
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
	nlohmann::json::iterator itGroup = root.find(filename);
	//未登録チェック
	if (itGroup == root.end()) {
		MessageBox(nullptr, L"ファイルの構造が正しくありません。", L"Map Editor - Load", 0);
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
							mapObject->model->position_ = { itItemObject->at(0), itItemObject->at(1), itItemObject->at(2) };
						}
						//名前がrotationだった場合、rotationを登録
						else if (itemNameObject == "rotation") {
							//float型のjson配列登録
							mapObject->model->rotation_ = { itItemObject->at(0), itItemObject->at(1), itItemObject->at(2) };
						}
						//名前がscaleだった場合、scaleを登録
						else if (itemNameObject == "scale") {
							//float型のjson配列登録
							mapObject->model->scale_ = { itItemObject->at(0), itItemObject->at(1), itItemObject->at(2) };
						}

					}
					//Vector3以外の場合
					else {

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
			player_->SetPosition(object->model->position_);
		}

		if (object->tag == "block") {
			std::shared_ptr<Block> newBlock = std::make_shared<Block>();
			newBlock->Initialize(Block::Color::kNone);
			newBlock->SetPosition(object->model->position_);
			newBlock->SetScale(object->model->scale_);
			blocks_.push_back(newBlock);
		}

		if (object->tag == "block_red") {
			std::shared_ptr<Block> newBlock = std::make_shared<Block>();
			newBlock->Initialize(Block::Color::kRed);
			newBlock->SetPosition(object->model->position_);
			newBlock->SetScale(object->model->scale_);
			blocks_.push_back(newBlock);
		}

		if (object->tag == "block_green") {
			std::shared_ptr<Block> newBlock = std::make_shared<Block>();
			newBlock->Initialize(Block::Color::kGreen);
			newBlock->SetPosition(object->model->position_);
			newBlock->SetScale(object->model->scale_);
			blocks_.push_back(newBlock);
		}

		if (object->tag == "block_blue") {
			std::shared_ptr<Block> newBlock = std::make_shared<Block>();
			newBlock->Initialize(Block::Color::kBlue);
			newBlock->SetPosition(object->model->position_);
			newBlock->SetScale(object->model->scale_);
			blocks_.push_back(newBlock);
		}

		if (object->tag == "block_yellow") {
			std::shared_ptr<Block> newBlock = std::make_shared<Block>();
			newBlock->Initialize(Block::Color::kYellow);
			newBlock->SetPosition(object->model->position_);
			newBlock->SetScale(object->model->scale_);
			blocks_.push_back(newBlock);
		}

		if (object->tag == "goal_red") {
			std::shared_ptr<Goal> newGoal = std::make_shared<Goal>();
			newGoal->Initialize(Goal::Color::kRed);
			newGoal->SetPosition(object->model->position_);
			goals_.push_back(newGoal);
		}

		if (object->tag == "goal_green") {
			std::shared_ptr<Goal> newGoal = std::make_shared<Goal>();
			newGoal->Initialize(Goal::Color::kGreen);
			newGoal->SetPosition(object->model->position_);
			goals_.push_back(newGoal);
		}

		if (object->tag == "goal_blue") {
			std::shared_ptr<Goal> newGoal = std::make_shared<Goal>();
			newGoal->Initialize(Goal::Color::kBlue);
			newGoal->SetPosition(object->model->position_);
			goals_.push_back(newGoal);
		}

		if (object->tag == "goal_yellow") {
			std::shared_ptr<Goal> newGoal = std::make_shared<Goal>();
			newGoal->Initialize(Goal::Color::kYellow);
			newGoal->SetPosition(object->model->position_);
			goals_.push_back(newGoal);
		}

	}

}
