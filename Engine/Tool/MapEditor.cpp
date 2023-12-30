#include "MapEditor.h"
#include "Externals/imgui/imgui.h"
#include "Externals/nlohmann/json.hpp"
#include <stdio.h>
#include <fstream>
#include <sstream>
#include <iostream>
#include <cassert>
#include <filesystem>

MapEditor* MapEditor::GetInstance() {
	static MapEditor instance;
	return &instance;
}

void MapEditor::Initialize() {

	for (const auto& tag : tagData_) {

		tags_.push_back(tag.c_str());

	}

}

void MapEditor::Edit() {

	ImGui::Begin("Map Editor");

	//ファイルを開いている時の処理
	if (isOpenFile_) {

		if (ImGui::Button("Save")) {
			Save(fileName_);
		}

		if (ImGui::Button("Close")) {
			Close();
		}

		ImGui::InputText("Object name", name_, sizeof(name_));

		if (ImGui::Button("Add Object")) {

			if (!CheckIsEmpty(name_)) {
				AddObject(name_);
				isSave_ = false;
			}
			else {
				MessageBox(nullptr, L"オブジェクト名を入力してください。", L"Map Editor - Add Object", 0);
			}
		}

		ImGui::InputText("Tag name", tagName_, sizeof(tagName_));

		if (ImGui::Button("Add Tag")) {

			if (!CheckIsEmpty(tagName_)) {
				AddTag(tagName_);
				isSave_ = false;
			}
			else {
				MessageBox(nullptr, L"タグ名を入力してください。", L"Map Editor - Add Tag", 0);
			}

		}

		for (auto& mapObjectData : mapObjData_) {

			std::string showObjName = mapObjectData->objName.c_str();
			showObjName += " ";

			if (ImGui::TreeNode(showObjName.c_str())) {

				if (ImGui::DragFloat3("position", &mapObjectData->model->position_.x, 0.1f)) {
					isSave_ = false;
				}

				if (ImGui::DragFloat3("rotation", &mapObjectData->model->rotation_.x, 0.01f)) {
					isSave_ = false;
				}

				if (ImGui::DragFloat3("scale", &mapObjectData->model->scale_.x, 0.01f)) {
					isSave_ = false;
				}

				if (ImGui::Combo("tag", &mapObjectData->tagNumber, tags_.data(), tags_.size())) {
					mapObjectData->tag = tags_[mapObjectData->tagNumber];
					isSave_ = false;
				}

				if (ImGui::Combo("mesh", &mapObjectData->meshNumber, meshNames_.data(), meshNames_.size())) {
					mapObjectData->meshName = meshNames_[mapObjectData->meshNumber];
					ChangeMesh(mapObjectData->model.get(), mapObjectData->meshName);
					isSave_ = false;
				}

				ImGui::TreePop();
			}

		}

	}
	else {

		ImGui::InputText(".json", fileName_, sizeof(fileName_));

		if (ImGui::Button("Create")) {

			//ファイル名が空の場合スキップ
			if (!CheckIsEmpty(fileName_)) {
				Create(fileName_);
			}
			else {
				MessageBox(nullptr, L"ファイル名を入力してください。", L"Map Editor - Create", 0);
			}

		}

		if (ImGui::Button("Load")) {

			//ファイル名が空の場合スキップ
			if (!CheckIsEmpty(fileName_)) {
				Load(fileName_);
			}
			else {
				MessageBox(nullptr, L"ファイル名を入力してください。", L"Map Editor - Load", 0);
			}

		}

	}

	ImGui::End();

}

void MapEditor::Draw(Camera* camera) {

	for (auto& mapObjData : mapObjData_) {

		mapObjData->model->Draw(camera);

	}

}

void MapEditor::Save(const std::string& filename) {

	nlohmann::json root;

	root = nlohmann::json::object();

	root[filename] = nlohmann::json::object();

	//設定したタグを保存
	nlohmann::json tagArray;

	for (const auto& tag : tagData_) {
		tagArray += tag;
	}

	root[filename]["tags"] = tagArray;

	for (auto& mapObjectData : mapObjData_) {

		root[filename]["objectData"][mapObjectData->objName]["position"] =
			nlohmann::json::array({ mapObjectData->model->position_.x, mapObjectData->model->position_.y, mapObjectData->model->position_.z });
		root[filename]["objectData"][mapObjectData->objName]["rotation"] =
			nlohmann::json::array({ mapObjectData->model->rotation_.x, mapObjectData->model->rotation_.y, mapObjectData->model->rotation_.z });
		root[filename]["objectData"][mapObjectData->objName]["scale"] =
			nlohmann::json::array({ mapObjectData->model->scale_.x, mapObjectData->model->scale_.y, mapObjectData->model->scale_.z });
		mapObjectData->tag = tags_[mapObjectData->tagNumber];
		root[filename]["objectData"][mapObjectData->objName]["tag"] = mapObjectData->tag;
		root[filename]["objectData"][mapObjectData->objName]["tagNumber"] = mapObjectData->tagNumber;
		mapObjectData->meshName = meshNames_[mapObjectData->meshNumber];
		root[filename]["objectData"][mapObjectData->objName]["mesh"] = mapObjectData->meshName;
		root[filename]["objectData"][mapObjectData->objName]["meshNumber"] = mapObjectData->meshNumber;

	}

	//ディレクトリが無ければ作成する
	std::filesystem::path dir(kDirectoryPath_);
	if (!std::filesystem::exists(dir)) {
		std::filesystem::create_directory(dir);
	}

	//書き込むJSONファイルのフルパスを合成する
	std::string filePath = kDirectoryPath_ + filename + ".json";
	//書き込み用ファイルストリーム
	std::ofstream ofs;
	//ファイルを書き込み用に開く
	ofs.open(filePath);

	//ファイルオープン失敗したら表示
	if (ofs.fail()) {
		MessageBox(nullptr, L"ファイルを開くのに失敗しました。", L"Map Editor - Save", 0);
		return;
	}

	//ファイルにjson文字列を書き込む(インデント幅4)
	ofs << std::setw(4) << root << std::endl;
	//ファイルを閉じる
	ofs.close();

	MessageBox(nullptr, L"セーブしました。", L"Map Editor - Save", 0);

	isSave_ = true;

}

void MapEditor::Close() {

	if (!isSave_) {

		auto id = MessageBox(
			nullptr,
			L"ファイルが保存されていません。保存しますか？",
			L"Map Editor - Close",
			MB_YESNOCANCEL);

		// OKの場合はセーブ
		if (id == IDYES) {
			Save(fileName_);
		}
		// キャンセルした場合早期リターン
		else if (id == IDCANCEL) {
			return;
		}
		else if (id == IDNO) {
			// 何もしない
		}

	}

	mapObjData_.clear();

	tagData_.clear();

	tagData_ = { "None" };

	tags_.clear();

	tags_ = { "None" };

	isOpenFile_ = false;

}

void MapEditor::Load(const std::string& filename) {

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
							ChangeMesh(mapObject->model.get(), mapObject->meshName);
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
		//追加したタグを登録
		else if (itemName == "tags") {
			
			/*tagData_.clear();*/

			tagData_ = root[filename]["tags"];

			tags_.clear();

			for (const auto& tag : tagData_) {

				tags_.push_back(tag.c_str());

			}

		}
	}

	isOpenFile_ = true;

}

void MapEditor::Create(const std::string& filename) {

	//読み込むJSONファイルのフルパスを合成する
	std::string filePath = kDirectoryPath_ + filename + ".json";

	std::filesystem::path path(filePath);

	//ファイルパスが存在するか確認
	if (std::filesystem::exists(path)) {

		if (MessageBox(nullptr, L"同名ファイルが既にあります。上書きしますか？", L"Map Editor - Create", MB_OKCANCEL) == IDCANCEL) {

			return;

		}

	}

	std::ofstream newFile(filePath);

	//新規ファイル作成
	if (newFile.is_open()) {
		newFile.close();
	}
	else {
		MessageBox(nullptr, L"ファイルを作成できませんでした。", L"Map Editor - Create", 0);
	}

	isOpenFile_ = true;

}

void MapEditor::AddObject(char* name) {

	std::string objectName = name;

	objectName = CheckSameName(objectName);

	std::shared_ptr<MapObject> mapObject = std::make_shared<MapObject>();

	mapObject->isSelect = true;
	mapObject->model.reset(Model::Create("./resources/cube/cube.obj"));
	mapObject->objName = objectName;
	mapObject->model->position_ = spawnPoint_;

	mapObjData_.push_back(mapObject);

}

void MapEditor::AddTag(const std::string& tagname) {

	if (CheckSameTag(tagname)) {

		MessageBox(nullptr, L"既に同名のタグがあります。", L"Map Editor - Add Tag", 0);

		return;

	}

	tagData_.push_back(tagname);

	tags_.clear();

	for (const auto& tag : tagData_) {

		tags_.push_back(tag.c_str());

	}

	

}

std::string MapEditor::CheckSameName(std::string name, uint32_t addNumber) {

	std::string newName = name;

	if (addNumber != 0) {
		newName += std::to_string(addNumber);
	}

	for (auto& mapObjData : mapObjData_) {

		if (mapObjData->objName.c_str() == newName) {
			newName = CheckSameName(name, addNumber + 1);
			break;
		}

	}

	return newName;

}

bool MapEditor::CheckSameTag(const std::string& name) {

	std::string newName = name;

	for (auto& tag : tagData_) {

		if (tag.c_str() == newName) {
			return true;
		}

	}

	return false;

}

void MapEditor::ChangeMesh(Model* model, const std::string& name) {

	std::string newMeshName = "./resources/";
	newMeshName += name + "/" + name;
	newMeshName += ".obj";
	model->SetMesh(newMeshName);

}

bool MapEditor::CheckIsEmpty(const std::string& name) {

	if (name.empty()) {
		return true;
	}

	return false;

}
