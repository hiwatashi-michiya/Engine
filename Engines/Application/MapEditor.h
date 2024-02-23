#pragma once
#include <list>
#include <memory>
#include "Engine/3d/Model.h"
#include "Engine/Tool/GlobalVariables.h"
#include <string>
#include <vector>

class MapEditor
{
public:
	
	static MapEditor* GetInstance();

	void Initialize();

	void Edit();

	void Draw(Camera* camera);

	const std::string& kDirectoryPath_ = "./resources/Maps/";

	void SetCamera(Camera* camera) { camera_ = camera; }

//関数,構造体
private:

	//マップに配置されているオブジェクトの構造体
	struct MapObject {

		std::string objName;

		std::unique_ptr<Model> model;

		//メッシュ
		std::string meshName;

		int32_t meshNumber = 0;

		//オブジェクトの役割を表すタグ
		std::string tag;

		int32_t tagNumber = 0;

		bool isSelect = false;

		//削除に使用するフラグ
		bool isDelete = false;

	};

	void Create(const std::string& filename);

	void Save(const std::string& filename);

	void Close();

	void Load(const std::string& filename);

	void AddObject(char* name);

	void CopyObject(std::shared_ptr<MapObject> object);

	bool CheckIsEmpty(const std::string& name);

	void AddTag(const std::string& tagname);

	//同名オブジェクトを探す
	std::string CheckSameName(std::string name, uint32_t addNumber = 0);

	//同名タグを探す
	bool CheckSameTag(const std::string& name);

	void ChangeMesh(Model* model, const std::string& name);

	void EditTransform();

//変数
private:

	Camera* camera_ = nullptr;

	GlobalVariables* globalVariables_ = nullptr;

	std::list<std::shared_ptr<MapObject>> mapObjData_;

	std::string currentObject_;

	bool isOpenFile_ = false;

	Vector3 spawnPoint_ = { 0.0f,0.0f,0.0f };

	char name_[256];

	char fileName_[256];

	bool isSave_ = true;

	char tagName_[256];

	std::vector<std::string> tagData_ = { "None" };

	std::vector<const char*> tags_;

	std::vector<const char*> meshNames_ = { "cube", "sphere", "plane" };

private:

	MapEditor() = default;
	~MapEditor() = default;
	MapEditor(const MapEditor&) = delete;
	const MapEditor& operator=(const MapEditor&) = delete;

};
