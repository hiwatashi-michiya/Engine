#pragma once
#include <vector>
#include <string>
#include <memory>
#include "Vector3.h"
#include "Externals/nlohmann/json.hpp"

class LevelData
{
public:

	struct ObjectData {
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


class LevelDataLoader
{
public:

	static LevelDataLoader* GetInstance();

	void Load(const std::string& fileName);

	void SetObjects();

	std::shared_ptr<LevelData> levelData_;

private:

	void ScanObject(nlohmann::json objects, const std::string& name);

private:


	LevelDataLoader() = default;
	~LevelDataLoader() = default;
	LevelDataLoader(const LevelDataLoader&) = delete;
	const LevelDataLoader& operator=(const LevelDataLoader&) = delete;

};
