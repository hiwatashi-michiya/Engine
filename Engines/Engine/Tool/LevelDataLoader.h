#pragma once
#include <vector>
#include <string>
#include <memory>
#include "Vector3.h"
#include "Externals/nlohmann/json.hpp"

/// <summary>
/// レベルの情報
/// </summary>
class LevelData
{
public:
	//オブジェクトの情報
	struct ObjectData {
		std::string type;
		std::string fileName;
		Vector3 translation;
		Vector3 rotation;
		Vector3 scaling;

	};


public:
	LevelData();
	~LevelData();


	std::vector<ObjectData> objects_;


};

/// <summary>
/// レベルデータを読み込むクラス
/// </summary>
class LevelDataLoader
{
public:
	//インスタンス取得
	static LevelDataLoader* GetInstance();
	//ロード
	void Load(const std::string& fileName);
	//オブジェクトセット
	void SetObjects();

	std::shared_ptr<LevelData> levelData_;

private:
	//オブジェクト読み込み
	void ScanObject(nlohmann::json objects, const std::string& name);

private:

	//シングルトン化
	LevelDataLoader() = default;
	~LevelDataLoader() = default;
	LevelDataLoader(const LevelDataLoader&) = delete;
	const LevelDataLoader& operator=(const LevelDataLoader&) = delete;

};
