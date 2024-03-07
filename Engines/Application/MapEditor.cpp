#include "MapEditor.h"
#include "Drawing/ImGuiManager.h"
#include <json.hpp>
#include <stdio.h>
#include <fstream>
#include <sstream>
#include <iostream>
#include <cassert>
#include <filesystem>

void MapEditor::EditTransform()
{
	static ImGuizmo::MODE mCurrentGizmoMode(ImGuizmo::LOCAL);
	static bool useSnap = false;
	static float snap[3] = { 1.f, 1.f, 1.f };
	static float bounds[] = { -0.5f, -0.5f, -0.5f, 0.5f, 0.5f, 0.5f };
	static float boundsSnap[] = { 0.1f, 0.1f, 0.1f };
	static bool boundSizing = false;
	static bool boundSizingSnap = false;
	static ImGuizmo::OPERATION mCurrentGizmoOperation(ImGuizmo::TRANSLATE);

	ImGui::Begin("Gizmo Transform");

	for (auto& object : mapObjData_) {

		std::string showObjName = object->objName.c_str();
		showObjName += " ";

		if (ImGui::TreeNode(showObjName.c_str())) {

			float matrixTranslation[3], matrixRotation[3], matrixScale[3];
			ImGuizmo::DecomposeMatrixToComponents(reinterpret_cast<float*>(object->model->worldMatrix_.m), matrixTranslation, matrixRotation, matrixScale);
			ImGui::InputFloat3("Tr", matrixTranslation);
			ImGui::InputFloat3("Rt", matrixRotation);
			ImGui::InputFloat3("Sc", matrixScale);
			ImGuizmo::RecomposeMatrixFromComponents(matrixTranslation, matrixRotation, matrixScale, reinterpret_cast<float*>(object->model->worldMatrix_.m));
			
			ImGui::TreePop();

		}

	}

	ImGui::End();

	/*if (mCurrentGizmoOperation != ImGuizmo::SCALE)
	{
		if (ImGui::RadioButton("Local", mCurrentGizmoMode == ImGuizmo::LOCAL)) {
			mCurrentGizmoMode = ImGuizmo::LOCAL;
		}
		ImGui::SameLine();
		if (ImGui::RadioButton("World", mCurrentGizmoMode == ImGuizmo::WORLD)) {
			mCurrentGizmoMode = ImGuizmo::WORLD;
		}
	}
	ImGui::Checkbox("##UseSnap", &useSnap);
	ImGui::SameLine();

	switch (mCurrentGizmoOperation)
	{
	case ImGuizmo::TRANSLATE:
		ImGui::InputFloat3("Snap", &snap[0]);
		break;
	case ImGuizmo::ROTATE:
		ImGui::InputFloat("Angle Snap", &snap[0]);
		break;
	case ImGuizmo::SCALE:
		ImGui::InputFloat("Scale Snap", &snap[0]);
		break;
	}
	ImGui::Checkbox("Bound Sizing", &boundSizing);
	if (boundSizing)
	{
		ImGui::PushID(3);
		ImGui::Checkbox("##BoundSizing", &boundSizingSnap);
		ImGui::SameLine();
		ImGui::InputFloat3("Snap", boundsSnap);
		ImGui::PopID();
	}*/

	ImGuiIO& io = ImGui::GetIO();
	float viewManipulateRight = io.DisplaySize.x;
	float viewManipulateTop = 0;
	static ImGuiWindowFlags gizmoWindowFlags = 0;

	ImGui::SetNextWindowSize(ImVec2(400, 200), ImGuiCond_Appearing);
	ImGui::SetNextWindowPos(ImVec2(0, 520), ImGuiCond_Appearing);
	ImGui::PushStyleColor(ImGuiCol_WindowBg, (ImVec4)ImColor(0.35f, 0.3f, 0.3f));
	ImGui::Begin("Gizmo", 0, gizmoWindowFlags);
	ImGuizmo::SetDrawlist();
	float windowWidth = (float)ImGui::GetWindowWidth();
	float windowHeight = (float)ImGui::GetWindowHeight();
	ImGuizmo::SetRect(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y, windowWidth, windowHeight);
	viewManipulateRight = ImGui::GetWindowPos().x + windowWidth;
	viewManipulateTop = ImGui::GetWindowPos().y;
	ImGuiWindow* window = ImGui::GetCurrentWindow();
	gizmoWindowFlags = ImGui::IsWindowHovered() && ImGui::IsMouseHoveringRect(window->InnerRect.Min, window->InnerRect.Max) ? ImGuiWindowFlags_NoMove : 0;

	float* cameraView = reinterpret_cast<float*>(camera_->matView_.m);
	float* cameraProjection = reinterpret_cast<float*>(camera_->matProjection_.m);

	ImGuizmo::DrawGrid(cameraView, cameraProjection, reinterpret_cast<const float*>(Matrix4x4::Identity().m), 100.f);

	for (auto& object : mapObjData_) {
		
		Vector3 scale = object->transform->scale_;
		scale *= 2.0f;
		Vector3 rotate = object->transform->rotate_;
		Vector3 translate = object->transform->translate_;

		Matrix4x4 tmpMatrix = MakeAffineMatrix(scale, rotate, translate);

		float* matrix = reinterpret_cast<float*>(tmpMatrix.m);
		ImGuizmo::DrawCubes(cameraView, cameraProjection, matrix, 1);
		ImGuizmo::Manipulate(cameraView, cameraProjection, mCurrentGizmoOperation, mCurrentGizmoMode, 
			matrix, NULL, useSnap ? &snap[0] : NULL, boundSizing ? bounds : NULL, boundSizingSnap ? boundsSnap : NULL);

		uint32_t count = 0;

		for (uint32_t i = 0; i < 4; i++) {

			for (uint32_t j = 0; j < 4; j++) {
				tmpMatrix.m[i][j] = matrix[count];
				count++;
			}

		}



	}

	/*ImGuizmo::ViewManipulate(cameraView, 30.0f, ImVec2(viewManipulateRight - 128, viewManipulateTop), ImVec2(128, 128), 0x10101010);*/

	ImGui::End();
	ImGui::PopStyleColor(1);

}

MapEditor* MapEditor::GetInstance() {
	static MapEditor instance;
	return &instance;
}

void MapEditor::Initialize() {

	camera_ = nullptr;

	spawnPoint_ = { 0.0f,0.0f,0.0f };

	if (isOpenFile_) {
		Close();
	}

	tags_.clear();

	for (const auto& tag : tagData_) {

		tags_.push_back(tag.c_str());

	}

}

void MapEditor::Edit() {

	mapObjData_.remove_if([](auto& object) {

		if (object->isDelete) {
			return true;
		}

		return false;

		});

	EditTransform();

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

		for (auto& object : mapObjData_) {

			std::string showObjName = object->objName.c_str();
			showObjName += " ";

			if (ImGui::TreeNode(showObjName.c_str())) {

				if (ImGui::DragFloat3("position", &object->transform->translate_.x, 0.1f)) {
					isSave_ = false;
				}

				if (ImGui::DragFloat3("rotation", &object->transform->rotate_.x, 0.01f)) {
					isSave_ = false;
				}

				if (ImGui::DragFloat3("scale", &object->transform->scale_.x, 0.01f)) {
					isSave_ = false;
				}

				if (ImGui::Combo("tag", &object->tagNumber, tags_.data(), int(tags_.size()))) {
					object->tag = tags_[object->tagNumber];
					isSave_ = false;
				}

				if (ImGui::Combo("mesh", &object->meshNumber, meshNames_.data(), int(meshNames_.size()))) {
					object->meshName = meshNames_[object->meshNumber];
					ChangeMesh(object->model.get(), object->meshName);
					isSave_ = false;
				}

				if (ImGui::Button("Copy")) {
					CopyObject(object);
				}

				if (ImGui::Button("Delete")) {
					object->isDelete = true;
				}

				ImGui::TreePop();
			}

			object->transform->UpdateMatrix();
			object->model->SetWorldMatrix(object->transform->worldMatrix_);

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

	for (auto& object : mapObjData_) {

		object->model->Draw(camera);

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

	for (auto& object : mapObjData_) {

		root[filename]["objectData"][object->objName]["position"] =
			nlohmann::json::array({ object->transform->translate_.x, object->transform->translate_.y, object->transform->translate_.z });
		root[filename]["objectData"][object->objName]["rotation"] =
			nlohmann::json::array({ object->transform->rotate_.x, object->transform->rotate_.y, object->transform->rotate_.z });
		root[filename]["objectData"][object->objName]["scale"] =
			nlohmann::json::array({ object->transform->scale_.x, object->transform->scale_.y, object->transform->scale_.z });
		object->tag = tags_[object->tagNumber];
		root[filename]["objectData"][object->objName]["tag"] = object->tag;
		root[filename]["objectData"][object->objName]["tagNumber"] = object->tagNumber;
		object->meshName = meshNames_[object->meshNumber];
		root[filename]["objectData"][object->objName]["mesh"] = object->meshName;
		root[filename]["objectData"][object->objName]["meshNumber"] = object->meshNumber;

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

				std::shared_ptr<MapObject> object = std::make_shared<MapObject>();

				object->isSelect = true;
				object->model.reset(Model::Create("./resources/cube/cube.obj"));
				object->transform = std::make_unique<Transform>();
				object->objName = objectName;

				uint32_t roopCount = 0;

				for (nlohmann::json::iterator itItemObject = itData->begin(); itItemObject != itData->end(); ++itItemObject) {

					//アイテム名を取得
					const std::string& itemNameObject = itItemObject.key();

					//要素数3の配列であれば
					if (itItemObject->is_array() && itItemObject->size() == 3) {

						//名前がpositionだった場合、positionを登録
						if (itemNameObject == "position") {
							//float型のjson配列登録
							object->transform->translate_ = { itItemObject->at(0), itItemObject->at(1), itItemObject->at(2) };
						}
						//名前がrotationだった場合、rotationを登録
						else if (itemNameObject == "rotation") {
							//float型のjson配列登録
							object->transform->rotate_ = { itItemObject->at(0), itItemObject->at(1), itItemObject->at(2) };
						}
						//名前がscaleだった場合、scaleを登録
						else if (itemNameObject == "scale") {
							//float型のjson配列登録
							object->transform->scale_ = { itItemObject->at(0), itItemObject->at(1), itItemObject->at(2) };
						}

					}
					//Vector3以外の場合
					else {

						//タグを登録
						if (itemNameObject == "tag") {
							object->tag = itItemObject.value();
						}
						else if (itemNameObject == "tagNumber") {
							object->tagNumber = itItemObject->get<int32_t>();
						}
						//メッシュを登録
						else if (itemNameObject == "mesh") {
							object->meshName = itItemObject.value();
							ChangeMesh(object->model.get(), object->meshName);
						}
						else if (itemNameObject == "meshNumber") {
							object->meshNumber = itItemObject->get<int32_t>();
						}

					}

					roopCount++;

				}

				mapObjData_.push_back(object);

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
		return;
	}

	isOpenFile_ = true;

}

void MapEditor::AddObject(char* name) {

	std::string objectName = name;

	objectName = CheckSameName(objectName);

	std::shared_ptr<MapObject> object = std::make_shared<MapObject>();

	object->isSelect = true;
	object->model.reset(Model::Create("./resources/cube/cube.obj"));
	object->transform = std::make_unique<Transform>();
	object->objName = objectName;
	object->transform->translate_ = spawnPoint_;

	mapObjData_.push_back(object);

}

void MapEditor::CopyObject(std::shared_ptr<MapObject> object) {

	std::string objectName = object->objName;

	objectName = CheckSameName(objectName);

	std::shared_ptr<MapObject> tmpObject = std::make_shared<MapObject>();

	tmpObject->isSelect = true;
	tmpObject->model.reset(Model::Create("./resources/cube/cube.obj"));
	tmpObject->model->SetMesh(object->model->meshFilePath_);
	tmpObject->meshNumber = object->meshNumber;
	tmpObject->objName = objectName;
	tmpObject->transform = std::make_unique<Transform>();
	tmpObject->transform->translate_ = object->transform->translate_;
	tmpObject->transform->rotate_ = object->transform->rotate_;
	tmpObject->transform->scale_ = object->transform->scale_;
	tmpObject->tag = object->tag;
	tmpObject->tagNumber = object->tagNumber;

	mapObjData_.push_back(tmpObject);

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
