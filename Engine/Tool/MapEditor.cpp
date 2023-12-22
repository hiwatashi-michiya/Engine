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

		ImGui::InputText("Object name", name_, sizeof(name_));

		if (ImGui::Button("Add Object")) {
			AddObject(name_);
		}

		for (auto& mapObjectData : mapObjData_) {



			if (ImGui::TreeNode(mapObjectData->objName.c_str())) {

				ImGui::DragFloat3("position", &mapObjectData->model->position_.x, 0.1f);
				ImGui::DragFloat3("rotation", &mapObjectData->model->rotation_.x, 0.01f);
				ImGui::DragFloat3("scale", &mapObjectData->model->scale_.x, 0.01f);

				ImGui::TreePop();
			}

		}

	}
	else {



	}

	ImGui::End();

}

void MapEditor::Draw(Camera* camera) {

	for (auto& mapObjData : mapObjData_) {

		mapObjData->model->Draw(camera);

	}

}

void MapEditor::Save() {

	

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

	}

	//保険
	assert(fp != NULL);


	{

		std::shared_ptr<MapObject> mapObject = std::make_shared<MapObject>();

		//オブジェクトのデータを読み取りきるまで回す
		while (fscanf_s(fp, "%f,%f,%f,%f,%f,%f,%f,%f,%f,",
			&mapObject->model->position_.x, &mapObject->model->position_.y, &mapObject->model->position_.z,
			&mapObject->model->rotation_.x, &mapObject->model->rotation_.y, &mapObject->model->rotation_.z,
			&mapObject->model->scale_.x, &mapObject->model->scale_.y, &mapObject->model->scale_.z) != EOF)
		{
			mapObjData_.push_back(mapObject);
			mapObject.reset();
		}

	}
	

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
