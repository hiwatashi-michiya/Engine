#pragma once
#include "Drawing/LineDrawer.h"
#include "Engine/3d/Model.h"
#include "Game/Block/Block.h"
#include "Game/Player/Player.h"
#include <memory>
#include <string>
#include "Game/Gimmick/GhostBox.h"
#include "Game/Gimmick/Switch.h"
#include "Particle.h"
#include "Game/Camera/FollowCamera.h"
#include "ColorCounter.h"

/// <summary>
/// ゲームシーンのステージに関連したオブジェクトの管理をするクラス
/// </summary>
class Stage
{
public:
	Stage();
	~Stage();

	static GameColor::Color stageColor_;
	//初期化
	void Initialize();
	//更新
	void Update();
	//描画コマンド積む
	void Draw(Camera* camera);
	//ステージ読み込み
	void LoadStage(uint32_t stageNumber);
	//追従カメラセット
	void SetFollowCamera(FollowCamera* camera) { followCamera_ = camera; }
	//読み込み先のディレクトリパス
	const std::string& kDirectoryPath_ = "./resources/Maps/";
	//プレイヤー取得
	Player* GetPlayer() const { return player_.get(); }
	//クリアフラグ取得
	bool GetIsClear() const { return isClear_; }
	//カウンター描画
	void DrawCounter() { if (counter_) { counter_->Draw(); } }

private:


private:

	//マップに配置されているオブジェクトの構造体
	struct MapObject {

		std::string objName;

		std::unique_ptr<Model> model;

		std::vector<std::unique_ptr<Transform>> transforms_;

		//メッシュ
		std::string meshName;

		int32_t meshNumber = 0;

		//オブジェクトの役割を表すタグ
		std::string tag;

		int32_t tagNumber = 0;

		bool isRotateRight_ = true;

		bool isSelect = false;

		GameColor::Color color = GameColor::kWhite;

	};

	FollowCamera* followCamera_ = nullptr;
	//プレイヤー
	std::unique_ptr<Player> player_;
	//ステージ全体に書けるパーティクル
	std::unique_ptr<Particle> stageParticle_;
	//色カウンター
	std::unique_ptr<ColorCounter> counter_;

	//TODO:リストを一つにまとめる

	//オブジェクトリスト
	std::list<std::unique_ptr<MapObject>> mapObjData_;
	//ゲームオブジェクトリスト
	std::list<std::unique_ptr<GameObject>> gameObjects_;

	//線
	Segment line_{};

	//ステージパーティクルの最小、最大範囲
	Vector3 minParticleLange_ = { -50.0f,-10.0f,-50.0f };
	Vector3 maxParticleLange_ = { 50.0f,-5.0f,50.0f };

	//当たり判定表示するかどうか
	bool isShowHitBox_ = false;
	//クリアフラグ
	bool isClear_ = false;

};

