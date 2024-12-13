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

/// <summary>
/// 特定のゲーム用に特化したステージエディタ
/// </summary>
class UniqueEditor
{
public:
	//インスタンス取得
	static UniqueEditor* GetInstance();

	//このエディターを使って出力したシーンに付けるrootNode名
	static inline const std::string kSceneName_ = "marScene";
	//初期化
	void Initialize();
	//更新
	void Finalize();
	//描画
	void Edit();
	//描画
	void Draw(Camera* camera);
	//ファイルパス
	const std::string& kDirectoryPath_ = "./Resources/Maps/";
	//カメラセット
	void SetCamera(Camera* camera) { camera_ = camera; }
	//オブジェクトリスト取得
	std::vector<std::shared_ptr<MapObject>> GetObjects() const { return mapObjData_; }
	//エディタを動かしていたか取得
	bool GetIsMove() const { return isMove_; }
	//一フレーム前のエディタを動かしたフラグを取得
	bool GetPreIsMove() const { return preIsMove_; }
	//ファイルを開いているか取得
	bool GetIsOpenFile() const { return isOpenFile_; }

	//Undo、Redoのコマンドリスト取得
	std::stack<std::shared_ptr<ICommand>>& GetUndoCommands() { return undoCommands_; }
	std::stack<std::shared_ptr<ICommand>>& GetRedoCommands() { return redoCommands_; }

//関数,構造体
private:
	//ファイル作成
	void Create(const std::string& filename);
	//セーブ
	void Save(const std::string& filename);
	//閉じる
	void Close();
	//読み込み
	void Load(const std::string& filename);
	//オブジェクトのコピー
	void CopyObject(std::shared_ptr<MapObject> object);
	//文字列が空かどうか判定
	bool CheckIsEmpty(const std::string& name);

	//同名オブジェクトを探す
	std::string CheckSameName(std::string name, uint32_t addNumber = 0);
	//メッシュ切り替え
	void ChangeMesh(Model* model, const std::string& name);
	//全モデルファイル読み込み
	void LoadAllModelFile();
	//全マップ読み込み
	void LoadAllMaps();
	//Guizmoの更新処理
	void EditTransform();
	//オブジェクト作成
	void CreateObject(const std::string& name);

	//初期ステージ配置をセット
	void SetDefaultStage();

	//リストから自分と一番近いオブジェクトを取り出す
	MapObject* GetNearObject(MapObject* self);

//変数
private:

	//使用するオブジェクトの最大数
	static const int32_t kMaxObjects_ = 11;
	//各オブジェクトの名前
	std::array<std::string, kMaxObjects_> objectName_ = 
	{ "player", "block", "moveBox", "paint", "goal", "warp", "ghostBox", "switch", "copyCat", "enemy", "colorHolder"};
	//入力に使う文字列
	char name_[256];
	//入力に使うファイルの名前文字列
	char fileName_[256];
	//メッシュのファイルパスとメッシュの名前を紐づけ
	std::unordered_map<std::string, std::string> meshMap_;
	//メッシュの名前と数字を紐づけ
	std::unordered_map<std::string, int32_t> meshNumMap_;

	//Undoできるコマンドリスト
	std::stack<std::shared_ptr<ICommand>> undoCommands_;
	//Redoできるコマンドリスト
	std::stack<std::shared_ptr<ICommand>> redoCommands_;
	//オブジェクトのリスト
	std::vector<std::shared_ptr<MapObject>> mapObjData_;
	//メッシュの名前
	std::vector<std::string> meshNames_;
	//マップの名前
	std::vector<std::string> mapNames_;
	//マウスのライン
	Segment mouseSegment_{};
	//オブジェクトを追加する時の座標
	Vector3 spawnPoint_ = { 0.0f,0.0f,0.0f };
	//一フレーム前のエディタ使用フラグ取得
	bool preIsMove_ = false;

	//オブジェクトの動きを記録するフラグ
	bool isRecordMove_ = false;
	//ロード中かどうか
	bool isLoading_ = false;
	//エディタを使用しているか
	bool isMove_ = false;
	//マウスの位置から飛ばす線
	std::unique_ptr<Line> mouseLine_;

	//古いトランスフォームの記録
	std::unique_ptr<Transform> oldTransform_;
	//カメラのポインタ
	Camera* camera_ = nullptr;
	//入力
	Input* input_ = nullptr;
	//現在選択しているオブジェクトの数字
	int32_t selectObject_ = 0;
	//ファイルを開いているかどうか
	bool isOpenFile_ = false;
	//セーブしているかどうか
	bool isSave_ = true;

private:
	//シングルトン化
	UniqueEditor() = default;
	~UniqueEditor() = default;
	UniqueEditor(const UniqueEditor&) = delete;
	const UniqueEditor& operator=(const UniqueEditor&) = delete;

};
