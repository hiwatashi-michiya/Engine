#include "GlobalVariables.h"
#include "Externals/nlohmann/json.hpp"
#include <Windows.h>

#ifdef _DEBUG
#include "Externals/imgui/imgui.h"
#endif // _DEBUG


GlobalVariables* GlobalVariables::GetInstance() {
	static GlobalVariables instance;
	return &instance;
}

void GlobalVariables::CreateGroup(const std::string& groupName) {

	//指定名のオブジェクトが無ければ追加する
	datas_[groupName];

}

void GlobalVariables::SetValue(
	const std::string& groupName,
	const std::string& key, int32_t value) {
	
	//グループの参照を取得
	Group& group = datas_[groupName];
	//新しい項目のデータを設定
	Item newItem{};
	newItem.value = value;
	//設定した項目をstd::mapに追加
	group.items[key] = newItem;
}

void GlobalVariables::SetValue(
	const std::string& groupName,
	const std::string& key, float value) {

	// グループの参照を取得
	Group& group = datas_[groupName];
	// 新しい項目のデータを設定
	Item newItem{};
	newItem.value = value;
	// 設定した項目をstd::mapに追加
	group.items[key] = newItem;

}

void GlobalVariables::SetValue(
	const std::string& groupName,
	const std::string& key, const Vector3& value) {

	// グループの参照を取得
	Group& group = datas_[groupName];
	// 新しい項目のデータを設定
	Item newItem{};
	newItem.value = value;
	// 設定した項目をstd::mapに追加
	group.items[key] = newItem;

}

void GlobalVariables::SetValue(
	const std::string& groupName,
	const std::string& key, const ObjectData& value) {

	// グループの参照を取得
	Group& group = datas_[groupName];
	// 新しい項目のデータを設定
	Item newItem{};
	newItem.value = value;
	// 設定した項目をstd::mapに追加
	group.items[key] = newItem;

}

void GlobalVariables::AddItem(const std::string& groupName,
	const std::string& key, int32_t value) {

	Group& group = datas_[groupName];
	if (group.items.find(key) == group.items.end()) {
		SetValue(groupName, key, value);
	}

}

void GlobalVariables::AddItem(const std::string& groupName,
	const std::string& key, float value) {

	Group& group = datas_[groupName];
	if (group.items.find(key) == group.items.end()) {
		SetValue(groupName, key, value);
	}

}

void GlobalVariables::AddItem(const std::string& groupName,
	const std::string& key, const Vector3& value) {

	Group& group = datas_[groupName];
	if (group.items.find(key) == group.items.end()) {
		SetValue(groupName, key, value);
	}

}

void GlobalVariables::AddItem(const std::string& groupName,
	const std::string& key, const ObjectData& value) {

	Group& group = datas_[groupName];
	if (group.items.find(key) == group.items.end()) {
		SetValue(groupName, key, value);
	}

}

int32_t GlobalVariables::GetIntValue(const std::string& groupName, const std::string& key) const {

	//指定グループが存在している
	assert(&datas_.at(groupName));
	//グループの参照を取得
	const Group& group = datas_.at(groupName);
	//指定グループに指定のキーが存在している
	assert(&group.items.at(key));
	//指定グループから指定のキーの値を取得
	const Item& item = group.items.at(key);
	return std::get<int32_t>(item.value);

}

float GlobalVariables::GetFloatValue(const std::string& groupName, const std::string& key) const {

	// 指定グループが存在している
	assert(&datas_.at(groupName));
	// グループの参照を取得
	const Group& group = datas_.at(groupName);
	// 指定グループに指定のキーが存在している
	assert(&group.items.at(key));
	// 指定グループから指定のキーの値を取得
	const Item& item = group.items.at(key);
	return std::get<float>(item.value);

}

Vector3 GlobalVariables::GetVector3Value(const std::string& groupName, const std::string& key) const {

	// 指定グループが存在している
	assert(&datas_.at(groupName));
	// グループの参照を取得
	const Group& group = datas_.at(groupName);
	// 指定グループに指定のキーが存在している
	assert(&group.items.at(key));
	// 指定グループから指定のキーの値を取得
	const Item& item = group.items.at(key);
	return std::get<Vector3>(item.value);

}

ObjectData GlobalVariables::GetObjectDataValue(const std::string& groupName, const std::string& key) const {

	// 指定グループが存在している
	assert(&datas_.at(groupName));
	// グループの参照を取得
	const Group& group = datas_.at(groupName);
	// 指定グループに指定のキーが存在している
	assert(&group.items.at(key));
	// 指定グループから指定のキーの値を取得
	const Item& item = group.items.at(key);
	return std::get<ObjectData>(item.value);

}

void GlobalVariables::Update() {

#ifdef _DEBUG


	if (!ImGui::Begin("Global Variables", nullptr, ImGuiWindowFlags_MenuBar)) {
		ImGui::End();
		return;
	}
	if (!ImGui::BeginMenuBar()) return;

	//各グループについて
	for (std::map<std::string, Group>::iterator itGroup = datas_.begin();
		itGroup != datas_.end(); ++itGroup) {

		//グループ名を取得
		const std::string& groupName = itGroup->first;
		//グループの参照を取得
		Group& group = itGroup->second;

		if (!ImGui::BeginMenu(groupName.c_str()))
			continue;

		//各項目について
		for (std::map<std::string, Item>::iterator itItem = group.items.begin();
			itItem != group.items.end(); ++itItem) {

			//項目名を取得
			const std::string& itemName = itItem->first;
			//項目の参照を取得
			Item& item = itItem->second;

			//int32_t型の値を保持していれば
			if (std::holds_alternative<int32_t>(item.value)) {

				int32_t* ptr = std::get_if<int32_t>(&item.value);
				ImGui::SliderInt(itemName.c_str(), ptr, 0, 100);

			}

			//float型の値を保持していれば
			else if (std::holds_alternative<float>(item.value)) {
				
				float* ptr = std::get_if<float>(&item.value);
				ImGui::SliderFloat(itemName.c_str(), ptr, 0.0f, 100.0f);

			}

			//Vector3型の値を保持していれば
			else if (std::holds_alternative<Vector3>(item.value)) {

				Vector3* ptr = std::get_if<Vector3>(&item.value);
				ImGui::SliderFloat3(itemName.c_str(), reinterpret_cast<float*>(ptr), -10.0f, 10.0f);

			}

			//ObjectData型の値を保持していれば
			else if (std::holds_alternative<ObjectData>(item.value)) {

				std::string strName = itemName + " : name";
				std::string posName = itemName + " : position";
				std::string rotName = itemName + " : rotation";
				std::string scaName = itemName + " : scale";

				ObjectData* ptr = std::get_if<ObjectData>(&item.value);
				char* strPtr = ptr->objName.data();
				Vector3* posPtr = &ptr->position;
				Vector3* rotPtr = &ptr->rotation;
				Vector3* scaPtr = &ptr->scale;
				ImGui::InputText(strName.c_str(), strPtr, 256);
				ImGui::SliderFloat3(posName.c_str(), reinterpret_cast<float*>(posPtr), -10.0f, 10.0f);
				ImGui::SliderFloat3(rotName.c_str(), reinterpret_cast<float*>(rotPtr), -10.0f, 10.0f);
				ImGui::SliderFloat3(scaName.c_str(), reinterpret_cast<float*>(scaPtr), -10.0f, 10.0f);

				//入力した文字列を代入
				ptr->objName = strPtr;

			}

		}

		//改行
		ImGui::Text("\n");

		if (ImGui::Button("Save")) {
			SaveFile(groupName);
			std::string message = std::format("{}.json saved", groupName);
			MessageBoxA(nullptr, message.c_str(), "GlobalVariables", 0);
		}

		ImGui::EndMenu();

	}

	ImGui::EndMenuBar();
	ImGui::End();

#endif // _DEBUG

}

void GlobalVariables::SaveFile(const std::string& groupName) {

	//グループを検索
	std::map<std::string, Group>::iterator itGroup = datas_.find(groupName);

	//未登録チェック
	assert(itGroup != datas_.end());

	nlohmann::json root;

	root = nlohmann::json::object();

	//jsonオブジェクト登録
	root[groupName] = nlohmann::json::object();

	//各項目について
	for (std::map<std::string, Item>::iterator itItem = itGroup->second.items.begin();
		itItem != itGroup->second.items.end(); ++itItem) {

		//項目名を取得
		const std::string& itemName = itItem->first;
		//項目の参照を取得
		Item& item = itItem->second;

		//int32_t型の値を保持していれば
		if (std::holds_alternative<int32_t>(item.value)) {
			//int32_t型の値を登録
			root[groupName][itemName] = std::get<int32_t>(item.value);
		}

		//float型の値を保持していれば
		else if (std::holds_alternative<float>(item.value)) {
			//float型の値を登録
			root[groupName][itemName] = std::get<float>(item.value);
		}

		//Vector3型を保持していれば
		else if (std::holds_alternative<Vector3>(item.value)) {
			//float型のjson配列登録
			Vector3 value = std::get<Vector3>(item.value);
			root[groupName][itemName] = nlohmann::json::array({value.x, value.y, value.z});
		}

		//ObjectData型を保持していれば
		else if (std::holds_alternative<ObjectData>(item.value)) {
			//float型のjson配列登録
			ObjectData value = std::get<ObjectData>(item.value);
			root[groupName][itemName]["name"] = value.objName;
			root[groupName][itemName]["position"] = nlohmann::json::array({value.position.x, value.position.y, value.position.z});
			root[groupName][itemName]["rotation"] = nlohmann::json::array({ value.rotation.x, value.rotation.y, value.rotation.z });
			root[groupName][itemName]["scale"] = nlohmann::json::array({ value.scale.x, value.scale.y, value.scale.z });
		}

	}

	//ディレクトリが無ければ作成する
	std::filesystem::path dir(kDirectoryPath);
	if (!std::filesystem::exists(dir)) {
		std::filesystem::create_directory(dir);
	}

	//書き込むJSONファイルのフルパスを合成する
	std::string filePath = kDirectoryPath + groupName + ".json";
	//書き込み用ファイルストリーム
	std::ofstream ofs;
	//ファイルを書き込み用に開く
	ofs.open(filePath);

	//ファイルオープン失敗したら表示
	if (ofs.fail()) {
		std::string message = "Failed open data file for write.";
		MessageBoxA(nullptr, message.c_str(), "GlobalVariables", 0);
		assert(0);
		return;
	}

	//ファイルにjson文字列を書き込む(インデント幅4)
	ofs << std::setw(4) << root << std::endl;
	//ファイルを閉じる
	ofs.close();

}

void GlobalVariables::LoadFiles() {

	std::filesystem::path dir(kDirectoryPath);
	//ディレクトリが無ければスキップする
	if (!std::filesystem::exists(dir)) {
		return;
	}

	std::filesystem::directory_iterator dir_it(kDirectoryPath);
	for (const std::filesystem::directory_entry& entry : dir_it) {

		//ファイルパスを取得
		const std::filesystem::path& filePath = entry.path();

		//ファイル拡張子を取得
		std::string extension = filePath.extension().string();
		//.jsonファイル以外はスキップ
		if (extension.compare(".json") != 0) {
			continue;
		}

		//ファイル読み込み
		LoadFile(filePath.stem().string());

	}

}

void GlobalVariables::LoadFile(const std::string& groupName) {

	//読み込むJSONファイルのフルパスを合成する
	std::string filePath = kDirectoryPath + groupName + ".json";
	//読み込み用ファイルストリーム
	std::ifstream ifs;
	//ファイルを読み込み用に開く
	ifs.open(filePath);

	//ファイルオープン失敗したら表示
	if (ifs.fail()) {
		std::string message = "Failed open data file for load.";
		MessageBoxA(nullptr, message.c_str(), "GlobalVariables", 0);
		assert(0);
		return;
	}

	nlohmann::json root;

	//json文字列からjsonのデータ構造に展開
	ifs >> root;
	//ファイルを閉じる
	ifs.close();
	//グループを検索
	nlohmann::json::iterator itGroup = root.find(groupName);
	//未登録チェック
	assert(itGroup != root.end());

	//各アイテムについて
	for (nlohmann::json::iterator itItem = itGroup->begin(); itItem != itGroup->end(); ++itItem) {
		
		//アイテム名を取得
		const std::string& itemName = itItem.key();

		//int32_t型の値を保持していれば
		if (itItem->is_number_integer()) {
			//int型の値を登録
			int32_t value = itItem->get<int32_t>();
			SetValue(groupName, itemName, value);
		}

		//float型の値を保持していれば
		else if (itItem->is_number_float()) {
			//float型の値を登録
			double value = itItem->get<double>();
			SetValue(groupName, itemName, static_cast<float>(value));
		}

		//要素数3の配列であれば
		else if (itItem->is_array() and itItem->size() == 3) {
			//float型のjson配列登録
			Vector3 value = {itItem->at(0), itItem->at(1), itItem->at(2)};
			SetValue(groupName, itemName, value);
		}

		//ObjectDataであれば
		else {
			//float型のjson配列登録
			ObjectData value;
			
			//グループを検索
			nlohmann::json::iterator itObject = itGroup->find(itemName);
			//未登録チェック
			assert(itObject != itGroup->end());

			uint32_t roopCount = 0;

			for (nlohmann::json::iterator itItemObject = itObject->begin(); itItemObject != itObject->end(); ++itItemObject) {

				//要素数3の配列であれば
				if (itItemObject->is_array() and itItemObject->size() == 3) {

					if (roopCount == 1) {
						//float型のjson配列登録
						value.position = { itItemObject->at(0), itItemObject->at(1), itItemObject->at(2) };
					}
					else if (roopCount == 2) {
						//float型のjson配列登録
						value.rotation = { itItemObject->at(0), itItemObject->at(1), itItemObject->at(2) };
					}
					else if (roopCount == 3) {
						//float型のjson配列登録
						value.scale = { itItemObject->at(0), itItemObject->at(1), itItemObject->at(2) };
					}

				}
				//文字列の場合
				else {
					value.objName = itItemObject.value();
				}

				roopCount++;

			}

			SetValue(groupName, itemName, value);
		}

	}

}
