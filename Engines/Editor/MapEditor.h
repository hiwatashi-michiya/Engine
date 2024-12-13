#pragma once
#include "Engine/3d/Model.h"
#include "Engine/Tool/GlobalVariables.h"
#include "Transform.h"
#include <list>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

/// <summary>
/// 汎用的なステージエディタ
/// </summary>
class MapEditor
{
public:
	//インスタンス取得
	static MapEditor* GetInstance();

	//このエディターを使って出力したシーンに付けるrootNode名
	static inline const std::string kSceneName_ = "marScene";
	//初期化
	void Initialize();
	//編集
	void Edit();
	//描画コマンド積む
	void Draw(Camera* camera);
	//出力先のファイルパス
	const std::string& kDirectoryPath_ = "./Resources/Maps/";
	//カメラセット
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

		//オブジェクトの役割を表すタグ
		std::string tag;

		int32_t meshNumber = 0;

		bool isSelect = false;

		//削除に使用するフラグ
		bool isDelete = false;

		bool isOpen_ = false;

		bool preOpen_ = false;


	};
	//生成
	void Create(const std::string& filename);
	//保存
	void Save(const std::string& filename);
	//閉じる
	void Close();
	//ロード
	void Load(const std::string& filename);
	//オブジェクト追加
	void AddObject(char* name);
	//オブジェクトコピー
	void CopyObject(std::shared_ptr<MapObject> object);
	//文字列が空かどうかのフラグ
	bool CheckIsEmpty(const std::string& name);

	//同名オブジェクトを探す
	std::string CheckSameName(std::string name, uint32_t addNumber = 0);
	//メッシュ切り替え
	void ChangeMesh(Model* model, const std::string& name);
	//全モデルファイルロード
	void LoadAllModelFile();
	//トランスフォーム編集
	void EditTransform();

//変数
private:

	Camera* camera_ = nullptr;

	GlobalVariables* globalVariables_ = nullptr;
	//オブジェクトのリスト
	std::list<std::shared_ptr<MapObject>> mapObjData_;

	//オブジェクトのトランスフォームのポインタを格納するマップ
	std::unordered_map<std::shared_ptr<MapObject>, float*> matrices_;
	//現在のオブジェクト
	std::string currentObject_;
	//ふぁいるを開いているかどうか
	bool isOpenFile_ = false;
	//セーブしたかどうか
	bool isSave_ = true;
	//オブジェクト生成座標
	Vector3 spawnPoint_ = { 0.0f,0.0f,0.0f };
	//名前
	char name_[256];
	//ファイル名
	char fileName_[256];
	//メッシュの名前
	std::vector<std::string> meshNames_;
	//メッシュのファイルパスと名前を紐づけたマップ
	std::unordered_map<std::string, std::string> meshMap_;
	//メッシュの名前と数字を紐づけたマップ
	std::unordered_map<std::string, int32_t> meshNumMap_;
	//現在選択しているオブジェクトの数字
	int32_t selectObject_ = 0;

private:
	//シングルトン化
	MapEditor() = default;
	~MapEditor() = default;
	MapEditor(const MapEditor&) = delete;
	const MapEditor& operator=(const MapEditor&) = delete;

};
