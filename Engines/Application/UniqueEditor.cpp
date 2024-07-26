#include "UniqueEditor.h"
#include "Drawing/ImGuiManager.h"
#include <json.hpp>
#include <stdio.h>
#include <fstream>
#include <sstream>
#include <iostream>
#include <cassert>
#include <filesystem>
#include <windows.h>
#include "Menu.h"
#include "UsefulFunc.h"

void UniqueEditor::EditTransform()
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
		
		if (auto warpPtr = dynamic_cast<WarpObject*>(object.get())) {

			Vector3 scale = warpPtr->transform_->scale_;
			Quaternion rotate = warpPtr->transform_->rotateQuaternion_;
			Vector3 translate = warpPtr->transform_->translate_;

			//どちらを動かすかによって渡す行列を変更する
			if (!warpPtr->isMoveA_) {
				scale = warpPtr->transformB_->scale_;
				rotate = warpPtr->transformB_->rotateQuaternion_;
				translate = warpPtr->transformB_->translate_;
			}

			Matrix4x4 tmpMatrix = MakeAffineMatrix(scale, rotate, translate);

			matrices.push_back(tmpMatrix);

		}
		else {

			Vector3 scale = object->transform_->scale_;
			Quaternion rotate = object->transform_->rotateQuaternion_;
			Vector3 translate = object->transform_->translate_;

			Matrix4x4 tmpMatrix = MakeAffineMatrix(scale, rotate, translate);

			matrices.push_back(tmpMatrix);

		}

	}

	//現在選択しているオブジェクトを動かすことが可能
	if (selectObject_ < mapObjData_.size()) {

		if (ImGuizmo::Manipulate(cameraView, cameraProjection, mCurrentGizmoOperation, mCurrentGizmoMode,
			reinterpret_cast<float*>(matrices[selectObject_].m), NULL, NULL, NULL, NULL)) {

		}

	}

	uint32_t count = 0;

	for (auto& object : mapObjData_) {

		if (auto warpPtr = dynamic_cast<WarpObject*>(object.get())) {

			if (warpPtr->isMoveA_) {

				warpPtr->transform_->UpdateMatrix();

				warpPtr->transform_->scale_ = matrices[count].GetScale();
				warpPtr->transform_->rotateQuaternion_ = ConvertFromRotateMatrix(matrices[count].GetRotateMatrix());
				warpPtr->transform_->translate_ = matrices[count].GetTranslate();

			}
			else {

				warpPtr->transformB_->UpdateMatrix();

				warpPtr->transformB_->scale_ = matrices[count].GetScale();
				warpPtr->transformB_->rotateQuaternion_ = ConvertFromRotateMatrix(matrices[count].GetRotateMatrix());
				warpPtr->transformB_->translate_ = matrices[count].GetTranslate();

			}

		}
		else {

			object->transform_->UpdateMatrix();

			object->transform_->scale_ = matrices[count].GetScale();
			object->transform_->rotateQuaternion_ = ConvertFromRotateMatrix(matrices[count].GetRotateMatrix());
			object->transform_->translate_ = matrices[count].GetTranslate();

		}

		count++;

	}

#endif // _DEBUG

}

UniqueEditor* UniqueEditor::GetInstance() {
	static UniqueEditor instance;
	return &instance;
}

void UniqueEditor::Initialize() {

	camera_ = nullptr;

	spawnPoint_ = { 0.0f,0.0f,0.0f };

	if (isOpenFile_) {
		Close();
	}

	LoadAllModelFile();

}

void UniqueEditor::Edit() {

#ifdef _DEBUG

	mapObjData_.remove_if([](auto& object) {

		if (object->isDelete_) {
			return true;
		}

		return false;

		});

	EditTransform();

	ImGui::Begin("Unique Editor");

	//ファイルを開いている時の処理
	if (isOpenFile_) {

		if (ImGui::Button("Save")) {
			Save(fileName_);
		}

		if (ImGui::Button("Close")) {
			Close();
		}

		if (ImGui::BeginTabBar("Objects", ImGuiTabBarFlags_None)) {

			for (int32_t i = 0; i < kMaxObjects_; i++) {

				if (ImGui::BeginTabItem(objectName_[i].c_str())) {

					if (objectName_[i] == "block") {

						if (ImGui::Button("Add")) {
							CreateObject(objectName_[i]);
						}

					}
					else if (objectName_[i] == "moveBox") {

						if (ImGui::Button("Add")) {
							CreateObject(objectName_[i]);
						}

					}
					else if (objectName_[i] == "ring") {

						if (ImGui::Button("Add")) {
							CreateObject(objectName_[i]);
						}

					}
					else if (objectName_[i] == "goal") {

						if (ImGui::Button("Add")) {
							CreateObject(objectName_[i]);
						}

					}
					else if (objectName_[i] == "warp") {

						if (ImGui::Button("Add")) {
							CreateObject(objectName_[i]);
						}

					}
					else if (objectName_[i] == "ghostBox") {

						if (ImGui::Button("Add")) {
							CreateObject(objectName_[i]);
						}

					}
					else if (objectName_[i] == "switch") {

						if (ImGui::Button("Add")) {
							CreateObject(objectName_[i]);
						}

					}

					for (int32_t k = 0; auto & object : mapObjData_) {

						object->preOpen_ = object->isOpen_;

						if (object->tag_ == objectName_[i]) {

							if (ImGui::TreeNode(object->objName_.c_str())) {

								object->isOpen_ = true;

								if (object->isOpen_ && !object->preOpen_) {
									selectObject_ = k;
								}

								object->Edit();

								if (object->tag_ != "player") {

									if (ImGui::Button("Copy")) {
										CopyObject(object);
									}

									if (ImGui::Button("Delete")) {

										object->isDelete_ = true;

										if (selectObject_ > 0) {
											selectObject_--;
										}

									}

								}

								ImGui::TreePop();

							}
							else {
								object->isOpen_ = false;
							}

						}

						object->transform_->UpdateMatrix();
						object->model_->SetWorldMatrix(object->transform_->worldMatrix_);

						if (auto warpPtr = dynamic_cast<WarpObject*>(object.get())) {
							warpPtr->transformB_->UpdateMatrix();
							warpPtr->modelB_->SetWorldMatrix(warpPtr->transformB_->worldMatrix_);
						}

						k++;

					}

					ImGui::EndTabItem();

				}

			}

			ImGui::EndTabBar();

		}

		if (!mapObjData_.empty()) {
			ImGui::SliderInt("Select Object", &selectObject_, 0, int(mapObjData_.size() - 1));
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

void UniqueEditor::Draw(Camera* camera) {

	for (auto& object : mapObjData_) {

		object->Draw(camera);

	}

}

void UniqueEditor::Save(const std::string& filename) {

	nlohmann::json root;

	root = nlohmann::json::object();

	root[sceneName_] = nlohmann::json::object();

	for (auto& object : mapObjData_) {

		if (auto warpPtr = dynamic_cast<WarpObject*>(object.get())) {

			root[sceneName_]["objectData"][warpPtr->objName_]["position"] =
				nlohmann::json::array({ warpPtr->transform_->translate_.x, warpPtr->transform_->translate_.y, warpPtr->transform_->translate_.z,
					warpPtr->transformB_->translate_.x, warpPtr->transformB_->translate_.y, warpPtr->transformB_->translate_.z });
			root[sceneName_]["objectData"][warpPtr->objName_]["quaternion"] =
				nlohmann::json::array({ warpPtr->transform_->rotateQuaternion_.x,
					warpPtr->transform_->rotateQuaternion_.y, warpPtr->transform_->rotateQuaternion_.z, warpPtr->transform_->rotateQuaternion_.w });
			root[sceneName_]["objectData"][warpPtr->objName_]["scale"] =
				nlohmann::json::array({ warpPtr->transform_->scale_.x, warpPtr->transform_->scale_.y, warpPtr->transform_->scale_.z });
			root[sceneName_]["objectData"][warpPtr->objName_]["tag"] = warpPtr->tag_;
			root[sceneName_]["objectData"][object->objName_]["color"] = object->colorNumber_;

		}
		else {

			root[sceneName_]["objectData"][object->objName_]["position"] =
				nlohmann::json::array({ object->transform_->translate_.x, object->transform_->translate_.y, object->transform_->translate_.z });
			root[sceneName_]["objectData"][object->objName_]["quaternion"] =
				nlohmann::json::array({ object->transform_->rotateQuaternion_.x,
					object->transform_->rotateQuaternion_.y, object->transform_->rotateQuaternion_.z, object->transform_->rotateQuaternion_.w });
			root[sceneName_]["objectData"][object->objName_]["scale"] =
				nlohmann::json::array({ object->transform_->scale_.x, object->transform_->scale_.y, object->transform_->scale_.z });
			root[sceneName_]["objectData"][object->objName_]["tag"] = object->tag_;
			root[sceneName_]["objectData"][object->objName_]["color"] = object->colorNumber_;

		}

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

void UniqueEditor::Close() {

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

void UniqueEditor::Load(const std::string& filename) {

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

				for (nlohmann::json::iterator itItemObject = itData->begin(); itItemObject != itData->end(); ++itItemObject) {

					//アイテム名を取得
					const std::string& itemNameObject = itItemObject.key();

					//タグによって対応した種類のオブジェクト生成
					if (itemNameObject == "tag") {
						CreateObject(itItemObject.value());
					}

				}

				//オブジェクトを取得
				auto object = mapObjData_.back();

				uint32_t roopCount = 0;

				for (nlohmann::json::iterator itItemObject = itData->begin(); itItemObject != itData->end(); ++itItemObject) {

					//アイテム名を取得
					const std::string& itemNameObject = itItemObject.key();

					//要素数3の配列であれば
					if (itItemObject->is_array() && itItemObject->size() == 3) {

						//名前がpositionだった場合、positionを登録
						if (itemNameObject == "position") {
							//float型のjson配列登録
							object->transform_->translate_ = { itItemObject->at(0), itItemObject->at(1), itItemObject->at(2) };
						}
						//名前がrotationだった場合、rotationを登録
						else if (itemNameObject == "rotation") {
							//float型のjson配列登録
							object->transform_->rotate_ = { itItemObject->at(0), itItemObject->at(1), itItemObject->at(2) };
							//クォータニオン更新
							object->transform_->rotateQuaternion_ = object->transform_->rotateQuaternion_.ConvertFromEuler(object->transform_->rotate_);
						}
						//名前がscaleだった場合、scaleを登録
						else if (itemNameObject == "scale") {
							//float型のjson配列登録
							object->transform_->scale_ = { itItemObject->at(0), itItemObject->at(1), itItemObject->at(2) };
						}

					}
					//Vector3以外の場合
					else {

						//名前がpositionだった場合、positionを登録
						if (itemNameObject == "position") {

							if (auto warpPtr = dynamic_cast<WarpObject*>(object.get())) {

								//float型のjson配列登録
								warpPtr->transform_->translate_ = { itItemObject->at(0), itItemObject->at(1), itItemObject->at(2) };
								warpPtr->transformB_->translate_ = { itItemObject->at(3), itItemObject->at(4), itItemObject->at(5) };

							}

						}

						if (itemNameObject == "color") {
							
							object->colorNumber_ = itItemObject->get<int32_t>();
							object->model_->SetColor(CreateColor(object->colorNumber_));

							if (auto warpPtr = dynamic_cast<WarpObject*>(object.get())) {

								warpPtr->modelB_->SetColor(CreateColor(warpPtr->colorNumber_));

							}

						}

						//クォータニオン追加
						if (itemNameObject == "quaternion") {
							//float型のjson配列登録
							object->transform_->rotateQuaternion_ = { itItemObject->at(0), itItemObject->at(1), itItemObject->at(2),itItemObject->at(3) };
						}

						//タグを登録
						else if (itemNameObject == "tag") {
							object->tag_ = itItemObject.value();
						}
						//メッシュを登録
						else if (itemNameObject == "mesh") {

						}
						/*else if (itemNameObject == "meshNumber") {
							object->meshNumber = itItemObject->get<int32_t>();
						}*/

					}

					roopCount++;

				}

				/*mapObjData_.push_back(object);*/

			}

		}

	}

	isOpenFile_ = true;

}

void UniqueEditor::Create(const std::string& filename) {

	//読み込むJSONファイルのフルパスを合成する
	std::string filePath = kDirectoryPath_ + filename + ".json";

	std::filesystem::path path(filePath);

	//ファイルパスが存在するか確認
	if (std::filesystem::exists(path)) {

		if (MessageBox(nullptr, L"同名ファイルが既にあります。上書きしますか？", L"Map Editor - Create", MB_OKCANCEL) == IDCANCEL) {

			return;

		}

	}

	//ディレクトリが無ければ作成する
	std::filesystem::path dir(kDirectoryPath_);
	if (!std::filesystem::exists(dir)) {
		std::filesystem::create_directory(dir);
	}

	std::ofstream newFile(filePath);

	//新規ファイル作成
	if (newFile.is_open()) {

		//jsonを読み込めるように空のシーン情報を記録
		nlohmann::json root;

		root = nlohmann::json::object();

		root[sceneName_] = nlohmann::json::object();

		//ファイルにjson文字列を書き込む(インデント幅4)
		newFile << std::setw(4) << root << std::endl;
		//ファイルを閉じる
		newFile.close();
	}
	else {
		MessageBox(nullptr, L"ファイルを作成できませんでした。", L"Map Editor - Create", 0);
		return;
	}

	SetDefaultStage();

	isOpenFile_ = true;

}

void UniqueEditor::CreateObject(const std::string& name) {

	if (name == "player") {

		std::string objectName = name;

		objectName = CheckSameName(objectName);

		std::shared_ptr<PlayerObject> object = std::make_shared<PlayerObject>();
		object->Initialize(objectName);
		mapObjData_.push_back(object);

	}
	else if (name == "block") {

		std::string objectName = name;

		objectName = CheckSameName(objectName);

		std::shared_ptr<BlockObject> object = std::make_shared<BlockObject>();
		object->Initialize(objectName);
		mapObjData_.push_back(object);

	}
	else if (name == "moveBox") {

		std::string objectName = name;

		objectName = CheckSameName(objectName);

		std::shared_ptr<MoveBoxObject> object = std::make_shared<MoveBoxObject>();
		object->Initialize(objectName);
		mapObjData_.push_back(object);

	}
	else if (name == "ring") {

		std::string objectName = name;

		objectName = CheckSameName(objectName);

		std::shared_ptr<RingObject> object = std::make_shared<RingObject>();
		object->Initialize(objectName);
		mapObjData_.push_back(object);

	}
	else if (name == "goal") {

		std::string objectName = name;

		objectName = CheckSameName(objectName);

		std::shared_ptr<GoalObject> object = std::make_shared<GoalObject>();
		object->Initialize(objectName);
		mapObjData_.push_back(object);

	}
	else if (name == "warp") {

		std::string objectName = name;

		objectName = CheckSameName(objectName);

		std::shared_ptr<WarpObject> object = std::make_shared<WarpObject>();
		object->Initialize(objectName);
		mapObjData_.push_back(object);

	}
	else if (name == "ghostBox") {

		std::string objectName = name;

		objectName = CheckSameName(objectName);

		std::shared_ptr<GhostBoxObject> object = std::make_shared<GhostBoxObject>();
		object->Initialize(objectName);
		mapObjData_.push_back(object);

	}
	else if (name == "switch") {

		std::string objectName = name;

		objectName = CheckSameName(objectName);

		std::shared_ptr<SwitchObject> object = std::make_shared<SwitchObject>();
		object->Initialize(objectName);
		mapObjData_.push_back(object);

	}
	else {

	}

}

void UniqueEditor::CopyObject(std::shared_ptr<MapObject> object) {

	std::string objectName = object->tag_;

	objectName = CheckSameName(objectName);

	if (object->tag_ == "player") {

		std::shared_ptr<PlayerObject> tmpObject = std::make_shared<PlayerObject>();
		tmpObject->Initialize(objectName);
		tmpObject->transform_->translate_ = object->transform_->translate_;
		tmpObject->transform_->rotate_ = object->transform_->rotate_;
		tmpObject->transform_->scale_ = object->transform_->scale_;
		mapObjData_.push_back(tmpObject);

	}
	else if (object->tag_ == "block") {

		std::shared_ptr<BlockObject> tmpObject = std::make_shared<BlockObject>();
		tmpObject->Initialize(objectName);
		tmpObject->transform_->translate_ = object->transform_->translate_;
		tmpObject->transform_->rotate_ = object->transform_->rotate_;
		tmpObject->transform_->scale_ = object->transform_->scale_;
		mapObjData_.push_back(tmpObject);

	}
	else if (object->tag_ == "moveBox") {

		std::shared_ptr<MoveBoxObject> tmpObject = std::make_shared<MoveBoxObject>();
		tmpObject->Initialize(objectName);
		tmpObject->transform_->translate_ = object->transform_->translate_;
		tmpObject->transform_->rotate_ = object->transform_->rotate_;
		tmpObject->transform_->scale_ = object->transform_->scale_;
		tmpObject->colorNumber_ = object->colorNumber_;
		mapObjData_.push_back(tmpObject);

	}
	else if (object->tag_ == "ring") {

		std::shared_ptr<RingObject> tmpObject = std::make_shared<RingObject>();
		tmpObject->Initialize(objectName);
		tmpObject->transform_->translate_ = object->transform_->translate_;
		tmpObject->transform_->rotate_ = object->transform_->rotate_;
		tmpObject->transform_->scale_ = object->transform_->scale_;
		tmpObject->colorNumber_ = object->colorNumber_;
		mapObjData_.push_back(tmpObject);

	}
	else if (object->tag_ == "goal") {

		std::shared_ptr<GoalObject> tmpObject = std::make_shared<GoalObject>();
		tmpObject->Initialize(objectName);
		tmpObject->transform_->translate_ = object->transform_->translate_;
		tmpObject->transform_->rotate_ = object->transform_->rotate_;
		tmpObject->transform_->scale_ = object->transform_->scale_;
		tmpObject->colorNumber_ = object->colorNumber_;
		mapObjData_.push_back(tmpObject);

	}
	else if (object->tag_ == "warp") {

		std::shared_ptr<WarpObject> tmpObject = std::make_shared<WarpObject>();
		tmpObject->Initialize(objectName);
		tmpObject->transform_->translate_ = object->transform_->translate_;
		tmpObject->transform_->rotate_ = object->transform_->rotate_;
		tmpObject->transform_->scale_ = object->transform_->scale_;

		if (auto warpPtr = dynamic_cast<WarpObject*>(object.get())) {
			tmpObject->transformB_->translate_ = warpPtr->transformB_->translate_;
			tmpObject->transformB_->rotate_ = warpPtr->transformB_->rotate_;
			tmpObject->transformB_->scale_ = warpPtr->transformB_->scale_;
		}

		tmpObject->colorNumber_ = object->colorNumber_;

		mapObjData_.push_back(tmpObject);

	}
	else if (object->tag_ == "ghostBox") {

		std::shared_ptr<GhostBoxObject> tmpObject = std::make_shared<GhostBoxObject>();
		tmpObject->Initialize(objectName);
		tmpObject->transform_->translate_ = tmpObject->transform_->translate_;
		tmpObject->transform_->rotate_ = tmpObject->transform_->rotate_;
		tmpObject->transform_->scale_ = tmpObject->transform_->scale_;
		tmpObject->colorNumber_ = object->colorNumber_;
		mapObjData_.push_back(tmpObject);

	}
	else if (object->tag_ == "switch") {

		std::shared_ptr<SwitchObject> tmpObject = std::make_shared<SwitchObject>();
		tmpObject->Initialize(objectName);
		tmpObject->transform_->translate_ = tmpObject->transform_->translate_;
		tmpObject->transform_->rotate_ = tmpObject->transform_->rotate_;
		tmpObject->transform_->scale_ = tmpObject->transform_->scale_;
		tmpObject->colorNumber_ = object->colorNumber_;
		mapObjData_.push_back(tmpObject);

	}

	else {

		std::shared_ptr<MapObject> tmpObject = std::make_shared<MapObject>();
		tmpObject->Initialize(objectName);
		tmpObject->transform_->translate_ = object->transform_->translate_;
		tmpObject->transform_->rotate_ = object->transform_->rotate_;
		tmpObject->transform_->scale_ = object->transform_->scale_;
		tmpObject->colorNumber_ = object->colorNumber_;
		mapObjData_.push_back(tmpObject);

	}

}

std::string UniqueEditor::CheckSameName(std::string name, uint32_t addNumber) {

	std::string newName = name;

	if (addNumber != 0) {
		newName += std::to_string(addNumber);
	}

	for (auto& mapObjData : mapObjData_) {

		if (mapObjData->objName_.c_str() == newName) {
			newName = CheckSameName(name, addNumber + 1);
			break;
		}

	}

	return newName;

}

void UniqueEditor::ChangeMesh(Model* model, const std::string& name) {

	model->SetMesh(meshMap_[name]);

}

void UniqueEditor::LoadAllModelFile() {

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

bool UniqueEditor::CheckIsEmpty(const std::string& name) {

	if (name.empty()) {
		return true;
	}

	return false;

}

void UniqueEditor::SetDefaultStage() {

	//プレイヤー生成
	{
		std::string objectName = "player";

		objectName = CheckSameName(objectName);

		std::shared_ptr<PlayerObject> object = std::make_shared<PlayerObject>();
		object->Initialize(objectName);
		mapObjData_.push_back(object);
	}
	//ブロック生成
	{
		std::string objectName = "block";

		objectName = CheckSameName(objectName);

		std::shared_ptr<BlockObject> object = std::make_shared<BlockObject>();
		object->Initialize(objectName);
		object->transform_->translate_ = { 0.0f,-2.0f,0.0f };
		object->transform_->scale_ = { 10.0f,1.0f,10.0f };
		mapObjData_.push_back(object);
	}
	{
		std::string objectName = "block";

		objectName = CheckSameName(objectName);

		std::shared_ptr<BlockObject> object = std::make_shared<BlockObject>();
		object->Initialize(objectName);
		object->transform_->translate_ = { 0.0f,-2.0f,30.0f };
		object->transform_->scale_ = { 10.0f,1.0f,10.0f };
		mapObjData_.push_back(object);
	}
	{
		std::string objectName = "block";

		objectName = CheckSameName(objectName);

		std::shared_ptr<BlockObject> object = std::make_shared<BlockObject>();
		object->Initialize(objectName);
		object->transform_->translate_ = { 30.0f,-2.0f,30.0f };
		object->transform_->scale_ = { 10.0f,1.0f,10.0f };
		mapObjData_.push_back(object);
	}
	{
		std::string objectName = "block";

		objectName = CheckSameName(objectName);

		std::shared_ptr<BlockObject> object = std::make_shared<BlockObject>();
		object->Initialize(objectName);
		object->transform_->translate_ = { 30.0f,-2.0f,0.0f };
		object->transform_->scale_ = { 10.0f,1.0f,10.0f };
		mapObjData_.push_back(object);
	}
	//リング生成
	{
		std::string objectName = "ring";

		objectName = CheckSameName(objectName);

		std::shared_ptr<RingObject> object = std::make_shared<RingObject>();
		object->Initialize(objectName);
		object->transform_->translate_ = { 0.0f,0.0f,30.0f };
		mapObjData_.push_back(object);
	}
	//動く箱生成
	{
		std::string objectName = "moveBox";

		objectName = CheckSameName(objectName);

		std::shared_ptr<MoveBoxObject> object = std::make_shared<MoveBoxObject>();
		object->Initialize(objectName);
		object->transform_->translate_ = { 30.0f,0.0f,0.0f };
		mapObjData_.push_back(object);
	}
	//ゴール生成
	{
		std::string objectName = "goal";

		objectName = CheckSameName(objectName);

		std::shared_ptr<GoalObject> object = std::make_shared<GoalObject>();
		object->Initialize(objectName);
		object->transform_->translate_ = { 30.0f,0.0f,30.0f };
		mapObjData_.push_back(object);
	}
	//ワープ生成
	{
		std::string objectName = "warp";

		objectName = CheckSameName(objectName);

		std::shared_ptr<WarpObject> object = std::make_shared<WarpObject>();
		object->Initialize(objectName);
		object->transform_->translate_ = { 5.0f,0.0f,5.0f };
		object->transformB_->translate_ = { 25.0f,0.0f,25.0f };
		mapObjData_.push_back(object);
	}

}
