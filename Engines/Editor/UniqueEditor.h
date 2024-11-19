#pragma once
#include "Engine/3d/Model.h"
#include "Engine/Tool/GlobalVariables.h"
#include "Transform.h"
#include <list>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>
#include "EditorObject.h"
#include "Input.h"
#include "LineDrawer.h"
#include <algorithm>
#include "EditorCommand.h"
#include <stack>

class UniqueEditor
{
public:
	
	static UniqueEditor* GetInstance();

	//このエディターを使って出力したシーンに付けるrootNode名
	static inline const std::string kSceneName_ = "marScene";

	void Initialize();

	void Finalize();

	void Edit();

	void Draw(Camera* camera);

	void DrawLine(Camera* camera);

	const std::string& kDirectoryPath_ = "./Resources/Maps/";

	void SetCamera(Camera* camera) { camera_ = camera; }

	std::vector<std::shared_ptr<MapObject>> GetObjects() const { return mapObjData_; }

	bool GetIsMove() const { return isMove_; }

	bool GetPreIsMove() const { return preIsMove_; }

	bool GetIsOpenFile() const { return isOpenFile_; }

	std::stack<std::shared_ptr<ICommand>>& GetUndoCommands() { return undoCommands_; }
	std::stack<std::shared_ptr<ICommand>>& GetRedoCommands() { return redoCommands_; }

//関数,構造体
private:

	void Create(const std::string& filename);

	void Save(const std::string& filename);

	void Close();

	void Load(const std::string& filename);

	void CopyObject(std::shared_ptr<MapObject> object);

	bool CheckIsEmpty(const std::string& name);

	//同名オブジェクトを探す
	std::string CheckSameName(std::string name, uint32_t addNumber = 0);

	void ChangeMesh(Model* model, const std::string& name);

	void LoadAllModelFile();

	void LoadAllMaps();

	void EditTransform();

	void CreateObject(const std::string& name);

	//初期ステージ配置をセット
	void SetDefaultStage();

	//リストから自分と一番近いオブジェクトを取り出す
	MapObject* GetNearObject(MapObject* self);

//変数
private:

	//使用するオブジェクトの最大数
	static const int32_t kMaxObjects_ = 11;

	std::array<std::string, kMaxObjects_> objectName_ = 
	{ "player", "block", "moveBox", "paint", "goal", "warp", "ghostBox", "switch", "copyCat", "enemy", "colorHolder"};

	Camera* camera_ = nullptr;

	Input* input_ = nullptr;

	GlobalVariables* globalVariables_ = nullptr;

	std::vector<std::shared_ptr<MapObject>> mapObjData_;

	std::string currentObject_;

	bool isOpenFile_ = false;

	Vector3 spawnPoint_ = { 0.0f,0.0f,0.0f };

	char name_[256];

	char fileName_[256];

	bool isSave_ = true;

	bool isLoading_ = false;

	bool isMove_ = false;

	bool preIsMove_ = false;

	std::vector<std::string> meshNames_;

	std::unordered_map<std::string, std::string> meshMap_;

	std::unordered_map<std::string, int32_t> meshNumMap_;

	std::vector<std::string> mapNames_;

	std::unique_ptr<Line> mouseLine_;

	int32_t selectObject_ = 0;

	Segment mouseSegment_{};

	//Undoできるコマンドリスト
	std::stack<std::shared_ptr<ICommand>> undoCommands_;
	//Redoできるコマンドリスト
	std::stack<std::shared_ptr<ICommand>> redoCommands_;
	//オブジェクトの動きを記録するフラグ
	bool isRecordMove_ = false;
	//古いトランスフォームの記録
	std::unique_ptr<Transform> oldTransform_;

private:

	UniqueEditor() = default;
	~UniqueEditor() = default;
	UniqueEditor(const UniqueEditor&) = delete;
	const UniqueEditor& operator=(const UniqueEditor&) = delete;

};
