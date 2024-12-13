#pragma once
#include "Particle.h"
#include "Camera.h"
#include <vector>
#include <string>
#include <json.hpp>
#include <unordered_map>

/// <summary>
/// パーティクルエフェクトの調整を行うエディタ
/// </summary>
class ParticleEditor
{
public:
	//このエディタで出力したデータに付ける名前
	static inline const std::string kParticleName_ = "marParticle";
	//出力先パス
	const std::string& kDirectoryPath_ = "./Resources/ParticleData/";
	//インスタンス取得
	static ParticleEditor* GetInstance();
	//初期化
	void Initialize();
	//終了処理
	void Finalize();
	//更新
	void Update();
	//描画
	void Draw(Camera* camera);
	//ファイルを開いているかどうか
	bool GetIsOpenFile() const { return isOpenFile_; }

private:
	//パーティクルの調整
	void EditParticle(Particle& particle);
	//読み込み
	void Load();
	//保存
	void Save();
	//パーティクル保存
	void SaveParticle(nlohmann::json& jsonData, Particle& particle);
	//作成
	void Create();
	//閉じる
	void Close();
	//名前が空かどうか
	bool CheckIsEmpty(const std::string& name);
	//画像読み込み
	void LoadPngFile();
	//パーティクルデータ読み込み
	void LoadAllData();

private:

	//作成中のパーティクル表示
	std::list<std::shared_ptr<Particle>> demoParticles_;
	//パーティクルデータの名前リスト
	std::vector<std::string> particleNames_;
	//画像の名前リスト
	std::vector<std::string> textureNames_;
	//画像のパスと画像の名前を紐づけ
	std::unordered_map<std::string, std::string> textureMap_;
	//画像の名前と数字を紐づけ
	std::unordered_map<std::string, int32_t> textureNumMap_;

	//パーティクルデータのファイル名
	char fileName_[256];
	//ファイル開いているかどうか
	bool isOpenFile_ = false;
	//セーブしたかどうか
	bool isSave_ = false;

private:
	//シングルトン化
	ParticleEditor() = default;
	~ParticleEditor() = default;
	ParticleEditor(const ParticleEditor&) = delete;
	const ParticleEditor& operator=(const ParticleEditor&) = delete;

};


