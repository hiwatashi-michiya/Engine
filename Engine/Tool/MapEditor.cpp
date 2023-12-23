#include "MapEditor.h"
#include "Externals/imgui/imgui.h"
#include <stdio.h>
#include <fstream>
#include <sstream>
#include <iostream>
#include <cassert>

MapEditor* MapEditor::GetInstance() {
	static MapEditor instance;
	return &instance;
}

void MapEditor::Initialize() {

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
			}
			else {
				MessageBox(nullptr, L"オブジェクト名を入力してください。", L"Map Editor - Add Object", 0);
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

				ImGui::TreePop();
			}

		}

	}
	else {

		ImGui::InputText(".csv", fileName_, sizeof(name_));

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

	FILE* fp = NULL;

	//ファイルパスの挿入
	std::string tmpFileName = "./resources/Maps/";

	tmpFileName += filename;

	tmpFileName += ".csv";

	//以前のファイルデータ削除、新規ファイル作成
	fopen_s(&fp, tmpFileName.c_str(), "w+b");

	if (fp == NULL) {

		MessageBox(nullptr, L"指定したファイルを開けませんでした。", L"Map Editor - Save", 0);

		return;

	}

	//保険
	assert(fp != NULL);

	//オブジェクト全てをファイルに保存
	for (auto& mapObjectData : mapObjData_) {

		fseek(fp, 0, SEEK_END);

		std::string str;

		str += mapObjectData->objName;
		str += "\n";
		str += std::to_string(mapObjectData->model->position_.x);
		str += ",";
		str += std::to_string(mapObjectData->model->position_.y);
		str += ",";
		str += std::to_string(mapObjectData->model->position_.z);
		str += ",\n";
		str += std::to_string(mapObjectData->model->rotation_.x);
		str += ",";
		str += std::to_string(mapObjectData->model->rotation_.y);
		str += ",";
		str += std::to_string(mapObjectData->model->rotation_.z);
		str += ",\n";
		str += std::to_string(mapObjectData->model->scale_.x);
		str += ",";
		str += std::to_string(mapObjectData->model->scale_.y);
		str += ",";
		str += std::to_string(mapObjectData->model->scale_.z);
		str += ",\n";

		//データをファイルに書き込む
		fputs(str.c_str(), fp);

	}

	fclose(fp);

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

	isOpenFile_ = false;

}

void MapEditor::Load(const std::string& filename) {

	FILE* fp = NULL;

	//ファイルパスの挿入
	std::string tmpFileName = "./resources/Maps/";

	tmpFileName += filename;

	tmpFileName += ".csv";

	fopen_s(&fp, tmpFileName.c_str(), "rb");

	//ファイルが開けなかったらメッセージ表示
	if (fp == NULL) {

		MessageBox(nullptr, L"指定したファイルは存在しません。", L"Map Editor - Load", 0);

		return;

	}

	//保険
	assert(fp != NULL);


	{

		char tmpObjName[256];

		//オブジェクトのデータを読み取りきるまで回す
		while (fscanf_s(fp, "%255s",&tmpObjName, static_cast<unsigned int>(sizeof(tmpObjName))) != EOF)
		{

			std::shared_ptr<MapObject> mapObject = std::make_shared<MapObject>();
			mapObject->isSelect = true;
			mapObject->model.reset(Model::Create("./resources/cube/cube.obj"));

			//モデルのSRT取得
			fscanf_s(fp, "%f,%f,%f,%f,%f,%f,%f,%f,%f,",
				&mapObject->model->position_.x, &mapObject->model->position_.y, &mapObject->model->position_.z,
				&mapObject->model->rotation_.x, &mapObject->model->rotation_.y, &mapObject->model->rotation_.z,
				&mapObject->model->scale_.x, &mapObject->model->scale_.y, &mapObject->model->scale_.z);

			mapObject->objName = tmpObjName;
			mapObjData_.push_back(mapObject);
		}

	}
	
	fclose(fp);

	isOpenFile_ = true;

}

void MapEditor::Create(const std::string& filename) {

	FILE* fp = NULL;

	//ファイルパスの挿入
	std::string tmpFileName = "./resources/Maps/";

	tmpFileName += filename;

	tmpFileName += ".csv";

	//作成する前に、同名ファイルがあるかチェック
	fopen_s(&fp, tmpFileName.c_str(), "rb");

	if (fp != NULL) {

		//作成しないならファイルをそのまま閉じて戻る
		if (MessageBox(nullptr, L"同名ファイルが既にあります。上書きしますか？", L"Map Editor - Create", MB_OKCANCEL) == IDCANCEL) {

			fclose(fp);

			return;

		}

		fclose(fp);

	}

	//新規ファイル作成
	fopen_s(&fp, tmpFileName.c_str(), "w");

	if (fp == NULL) {

		MessageBox(nullptr, L"ファイルの作成に失敗しました。", L"Map Editor - Create", 0);

		return;

	}

	//保険
	assert(fp != NULL);

	fclose(fp);

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

bool MapEditor::CheckIsEmpty(const std::string& name) {

	if (name.empty()) {
		return true;
	}

	return false;

}
