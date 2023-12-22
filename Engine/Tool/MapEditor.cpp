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

void MapEditor::Edit() {

	ImGui::Begin("Map Editor");

	//ファイルを開いている時の処理
	if (isOpenFile_) {

		if (ImGui::Button("Add Object")) {
			AddObject();
		}

		for (auto& mapObject : mapObjData_) {

			if (ImGui::TreeNode("obj")) {

				ImGui::DragFloat3("position", &mapObject->model->position_.x, 0.1f);
				ImGui::DragFloat3("rotation", &mapObject->model->rotation_.x, 0.01f);
				ImGui::DragFloat3("scale", &mapObject->model->scale_.x, 0.01f);

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
		while (fscanf(fp, "%s,%f,%f,%f,%f,%f,%f,%f,%f,%f,",
			mapObject->objName, 
			mapObject->model->position_.x, mapObject->model->position_.y, mapObject->model->position_.z,
			mapObject->model->rotation_.x, mapObject->model->rotation_.y, mapObject->model->rotation_.z,
			mapObject->model->scale_.x, mapObject->model->scale_.y, mapObject->model->scale_.z) != EOF)
		{
			mapObjData_.push_back(mapObject);
			mapObject.reset();
		}

	}
	

}

void MapEditor::AddObject() {

	std::shared_ptr<MapObject> mapObject = std::make_shared<MapObject>();

	mapObject->isSelect = true;
	mapObject->model.reset(Model::Create("./resources/cube/cube.obj"));
	mapObject->objName = "cube";
	mapObject->model->position_ = spawnPoint_;

	mapObjData_.push_back(mapObject);

}
