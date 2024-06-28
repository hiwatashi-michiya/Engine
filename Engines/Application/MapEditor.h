#pragma once
#include "Engine/3d/Model.h"
#include "Engine/Tool/GlobalVariables.h"
#include "Transform.h"
#include <list>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

class MapEditor
{
public:
	
	static MapEditor* GetInstance();

	//このエディターを使って出力したシーンに付けるrootNode名
	static inline const std::string sceneName_ = "marScene";

	void Initialize();

	void Edit();

	void Draw(Camera* camera);

	const std::string& kDirectoryPath_ = "./Resources/Maps/";

	void SetCamera(Camera* camera) { camera_ = camera; }

//関数,構造体
private:

	//マップに配置されているオブジェクトの構造体
	struct MapObject {

		std::string objName;

		std::unique_ptr<Model> model;

		std::unique_ptr<Transform> transform;

		float* matrix;

		//メッシュ
		std::string meshName;

		int32_t meshNumber = 0;

		//オブジェクトの役割を表すタグ
		std::string tag;

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

	//同名オブジェクトを探す
	std::string CheckSameName(std::string name, uint32_t addNumber = 0);

	void ChangeMesh(Model* model, const std::string& name);

	void LoadAllModelFile();

	void EditTransform();

//変数
private:

	Camera* camera_ = nullptr;

	GlobalVariables* globalVariables_ = nullptr;

	std::list<std::shared_ptr<MapObject>> mapObjData_;

	//オブジェクトのトランスフォームのポインタを格納するマップ
	std::unordered_map<std::shared_ptr<MapObject>, float*> matrices_;

	std::string currentObject_;

	bool isOpenFile_ = false;

	Vector3 spawnPoint_ = { 0.0f,0.0f,0.0f };

	char name_[256];

	char fileName_[256];

	bool isSave_ = true;

	std::vector<std::string> meshNames_;

	std::unordered_map<std::string, std::string> meshMap_;

	std::unordered_map<std::string, int32_t> meshNumMap_;

	int32_t selectObject_ = 0;

private:

	MapEditor() = default;
	~MapEditor() = default;
	MapEditor(const MapEditor&) = delete;
	const MapEditor& operator=(const MapEditor&) = delete;

};
