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



	}
	else {

	}

	ImGui::End();

}

void MapEditor::Draw() {

	for (ObjectData* objData : objData_) {

		if (objData->objName == "cube") {

		}
		else if (objData->objName == "sphere") {

		}

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

	

}

void MapEditor::AddObject() {



}
