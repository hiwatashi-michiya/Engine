#include "MapEditor.h"
#include "Drawing/ImGuiManager.h"
#include <json.hpp>
#include <stdio.h>
#include <fstream>
#include <sstream>
#include <iostream>
#include <cassert>
#include <filesystem>
#include <windows.h>

void MapEditor::EditTransform()
{

#ifdef _DEBUG




	//カメラが無い場合、処理をしない
	if (!camera_) {
		return;
	}

	static ImGuizmo::MODE mCurrentGizmoMode(ImGuizmo::LOCAL);
	static bool useSnap = false;
	static float snap[3] = { 1.f, 1.f, 1.f };
	static float bounds[] = { -0.5f, -0.5f, -0.5f, 0.5f, 0.5f, 0.5f };
	static float boundsSnap[] = { 0.1f, 0.1f, 0.1f };
	static bool boundSizing = false;
	static bool boundSizingSnap = false;
	static ImGuizmo::OPERATION mCurrentGizmoOperation(ImGuizmo::TRANSLATE);

	ImGui::Begin("Gizmo Transform");

	if (ImGui::RadioButton("Translate", mCurrentGizmoOperation == ImGuizmo::TRANSLATE)) {
		mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
	}
	ImGui::SameLine();
	if (ImGui::RadioButton("Rotate", mCurrentGizmoOperation == ImGuizmo::ROTATE)) {
		mCurrentGizmoOperation = ImGuizmo::ROTATE;
	}
	ImGui::SameLine();
	if (ImGui::RadioButton("Scale", mCurrentGizmoOperation == ImGuizmo::SCALE)) {
		mCurrentGizmoOperation = ImGuizmo::SCALE;
	}

	ImGui::End();

	ImGuizmo::SetRect(0, 0, 1280, 720);
	
	float* cameraView = reinterpret_cast<float*>(camera_->matView_.m);
	float* cameraProjection = reinterpret_cast<float*>(camera_->matProjection_.m);

	std::vector<Matrix4x4> matrices;

	for (auto& object : mapObjData_) {
		
		Vector3 scale = object->transform->scale_;
		Quaternion rotate = object->transform->rotateQuaternion_;
		Vector3 translate = object->transform->translate_;

		Matrix4x4 tmpMatrix = MakeAffineMatrix(scale, rotate, translate);

		matrices.push_back(tmpMatrix);

	}

	//現在選択しているオブジェクトを動かすことが可能
	if (selectObject_ < mapObjData_.size()) {

		if (ImGuizmo::Manipulate(cameraView, cameraProjection, mCurrentGizmoOperation, mCurrentGizmoMode,
			reinterpret_cast<float*>(matrices[selectObject_].m), NULL, NULL, NULL, NULL)) {

		}

	}

	uint32_t count = 0;

	for (auto& object : mapObjData_) {

		object->transform->UpdateMatrix();

		object->transform->scale_ = matrices[count].GetScale();
		object->transform->rotateQuaternion_ = ConvertFromRotateMatrix(matrices[count].GetRotateMatrix());
		object->transform->translate_ = matrices[count].GetTranslate();

		count++;

	}

#endif // _DEBUG

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

	LoadAllModelFile();

}

void MapEditor::Edit() {

#ifdef _DEBUG

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

		if (!mapObjData_.empty()) {
			ImGui::SliderInt("Select Object", &selectObject_, 0, int(mapObjData_.size() - 1));
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

				{

					//vectorを中間バッファとして利用
					std::vector<char> bufferStr(object->tag.begin(), object->tag.end());
					//リサイズする
					bufferStr.resize(256);

					//書き換えた文字列をtagに戻す
					if (ImGui::InputText("tag", bufferStr.data(), bufferStr.size())) {
						isSave_ = false;
						auto endIt = std::find(bufferStr.begin(), bufferStr.end(), '\0');
						object->tag = std::string(bufferStr.begin(), endIt);
					}

				}

				/*if (ImGui::Combo("tag", &object->tagNumber, tags_.data(), int(tags_.size()))) {
					object->tag = tags_[object->tagNumber];
					isSave_ = false;
				}*/

				std::vector<const char*> meshNames;

				for (uint32_t i = 0; i < meshNames_.size(); i++) {
					meshNames.push_back(meshNames_[i].c_str());
				}

				if (ImGui::Combo("mesh", &object->meshNumber, meshNames.data(), int(meshNames_.size()))) {
					object->meshName = meshNames_[object->meshNumber];
					ChangeMesh(object->model.get(), object->meshName);
					isSave_ = false;
				}

				if (ImGui::Button("Copy")) {
					CopyObject(object);
				}

				if (ImGui::Button("Delete")) {
					object->isDelete = true;
					matrices_.erase(object);
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

#endif // _DEBUG

}

void MapEditor::Draw(Camera* camera) {

	for (auto& object : mapObjData_) {

		object->model->Draw(camera);

	}

}

void MapEditor::Save(const std::string& filename) {

	nlohmann::json root;

	root = nlohmann::json::object();

	root[sceneName_] = nlohmann::json::object();

	//設定したタグを保存
	nlohmann::json tagArray;

	for (const auto& tag : tagData_) {
		tagArray += tag;
	}

	root[sceneName_]["tags"] = tagArray;

	for (auto& object : mapObjData_) {

		root[sceneName_]["objectData"][object->objName]["position"] =
			nlohmann::json::array({ object->transform->translate_.x, object->transform->translate_.y, object->transform->translate_.z });
		/*root[filename]["objectData"][object->objName]["rotation"] =
			nlohmann::json::array({ object->transform->rotate_.x, object->transform->rotate_.y, object->transform->rotate_.z });*/
		root[sceneName_]["objectData"][object->objName]["quaternion"] =
			nlohmann::json::array({ object->transform->rotateQuaternion_.x,
				object->transform->rotateQuaternion_.y, object->transform->rotateQuaternion_.z, object->transform->rotateQuaternion_.w });
		root[sceneName_]["objectData"][object->objName]["scale"] =
			nlohmann::json::array({ object->transform->scale_.x, object->transform->scale_.y, object->transform->scale_.z });
		/*object->tag = tags_[object->tagNumber];*/
		root[sceneName_]["objectData"][object->objName]["tag"] = object->tag;
		object->meshName = meshNames_[object->meshNumber];
		root[sceneName_]["objectData"][object->objName]["mesh"] = object->meshName;
		/*root[sceneName_]["objectData"][object->objName]["meshNumber"] = object->meshNumber;*/

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
	nlohmann::json::iterator itGroup = root.find(sceneName_);
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
				object->model.reset(Model::Create("./Resources/cube/cube.obj"));
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
							//クォータニオン更新
							object->transform->rotateQuaternion_ = object->transform->rotateQuaternion_.ConvertFromEuler(object->transform->rotate_);
						}
						//名前がscaleだった場合、scaleを登録
						else if (itemNameObject == "scale") {
							//float型のjson配列登録
							object->transform->scale_ = { itItemObject->at(0), itItemObject->at(1), itItemObject->at(2) };
						}

					}
					//Vector3以外の場合
					else {

						//クォータニオン追加
						if (itemNameObject == "quaternion") {
							//float型のjson配列登録
							object->transform->rotateQuaternion_ = { itItemObject->at(0), itItemObject->at(1), itItemObject->at(2),itItemObject->at(3) };
						}

						//タグを登録
						else if (itemNameObject == "tag") {
							object->tag = itItemObject.value();
						}
						//メッシュを登録
						else if (itemNameObject == "mesh") {
							object->meshName = itItemObject.value();
							object->meshNumber = meshNumMap_[object->meshName];
							ChangeMesh(object->model.get(), object->meshName);
						}
						/*else if (itemNameObject == "meshNumber") {
							object->meshNumber = itItemObject->get<int32_t>();
						}*/

					}

					roopCount++;

				}

				mapObjData_.push_back(object);

			}

		}
		//追加したタグを登録
		else if (itemName == "tags") {
			
			/*tagData_.clear();*/

			tagData_ = root[sceneName_]["tags"];

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
	object->model.reset(Model::Create("./Resources/cube/cube.obj"));
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
	tmpObject->model.reset(Model::Create("./Resources/cube/cube.obj"));
	tmpObject->model->SetMesh(object->model->meshFilePath_);
	tmpObject->meshNumber = object->meshNumber;
	tmpObject->objName = objectName;
	tmpObject->transform = std::make_unique<Transform>();
	tmpObject->transform->translate_ = object->transform->translate_;
	tmpObject->transform->rotate_ = object->transform->rotate_;
	tmpObject->transform->scale_ = object->transform->scale_;
	tmpObject->tag = object->tag;

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

	model->SetMesh(meshMap_[name]);

}

void MapEditor::LoadAllModelFile() {

	meshNames_.clear();
	meshMap_.clear();

	//リソース内のobjファイル全検索
	std::filesystem::recursive_directory_iterator itr("./Resources");

	//検索する拡張子
	std::string extension = ".obj";

	//ファイル内の.objのファイルを全検索
	for (const auto& entry : itr) {

		if (std::filesystem::is_regular_file(entry.path()) &&
			entry.path().extension() == extension) {
			std::string meshName = entry.path().stem().string();
			//名前と番号を紐づけ
			meshNumMap_[meshName] = int32_t(meshNames_.size());
			//最後尾に追加
			meshNames_.push_back(meshName);
			//名前とパスを紐づけ
			meshMap_[meshName] = entry.path().string();
		}

	}

	//イテレータリセット
	itr = std::filesystem::recursive_directory_iterator("./Resources");

	//検索する拡張子を.gltfに変更
	extension = ".gltf";

	//ファイル内の.gltfのファイルを全検索
	for (const auto& entry : itr) {

		if (std::filesystem::is_regular_file(entry.path()) &&
			entry.path().extension() == extension) {
			std::string meshName = entry.path().stem().string();
			//名前と番号を紐づけ
			meshNumMap_[meshName] = int32_t(meshNames_.size());
			//最後尾に追加
			meshNames_.push_back(meshName);
			//名前とパスを紐づけ
			meshMap_[meshName] = entry.path().string();
		}

	}

}

bool MapEditor::CheckIsEmpty(const std::string& name) {

	if (name.empty()) {
		return true;
	}

	return false;

}
