#pragma once
#include <list>
#include <memory>
#include "Engine/3d/Model.h"
#include "Engine/Tool/GlobalVariables.h"
#include <string>

class MapEditor
{
public:
	
	static MapEditor* GetInstance();

	void Initialize();

	void Edit();

	void Draw(Camera* camera);

//関数
private:

	void Save();

	void Load(const std::string& filename);

	void AddObject(char* name);

	//同名オブジェクトを探す
	std::string CheckSameName(std::string name, uint32_t addNumber = 0);

//変数
private:


	//マップに配置されているオブジェクトの構造体
	struct MapObject {

		std::string objName;

		std::unique_ptr<Model> model;

		bool isSelect = false;

	};

	GlobalVariables* globalVariables_ = nullptr;

	std::list<std::shared_ptr<MapObject>> mapObjData_;

	std::string currentObject_;

	bool isOpenFile_ = true;

	Vector3 spawnPoint_ = { 0.0f,0.0f,0.0f };

	char name_[256];

private:

	MapEditor() = default;
	~MapEditor() = default;
	MapEditor(const MapEditor&) = delete;
	const MapEditor& operator=(const MapEditor&) = delete;

};
