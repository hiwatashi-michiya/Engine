#pragma once
#include "Particle.h"
#include "Camera.h"
#include <vector>
#include <string>
#include <json.hpp>
#include <unordered_map>

class ParticleEditor
{
public:
	
	static inline const std::string kParticleName_ = "marParticle";

	const std::string& kDirectoryPath_ = "./Resources/ParticleData/";

	static ParticleEditor* GetInstance();

	void Initialize();

	void Finalize();

	void Update();

	void Draw(Camera* camera);

	bool GetIsOpenFile() const { return isOpenFile_; }

private:

	void EditParticle(Particle& particle);

	void Load();

	void Save();

	void SaveParticle(nlohmann::json& jsonData, Particle& particle);

	void Create();

	void Close();

	bool CheckIsEmpty(const std::string& name);

	void LoadPngFile();

	void LoadAllData();

private:

	//作成中のパーティクル表示
	std::list<std::shared_ptr<Particle>> demoParticles_;

	std::vector<std::string> particleNames_;

	std::vector<std::string> textureNames_;

	std::unordered_map<std::string, std::string> textureMap_;

	std::unordered_map<std::string, int32_t> textureNumMap_;

	//パーティクルデータのファイル名
	char fileName_[256];

	bool isOpenFile_ = false;

	bool isSave_ = false;

private:

	ParticleEditor() = default;
	~ParticleEditor() = default;
	ParticleEditor(const ParticleEditor&) = delete;
	const ParticleEditor& operator=(const ParticleEditor&) = delete;

};


