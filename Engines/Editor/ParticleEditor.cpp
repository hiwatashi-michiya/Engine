#include "ParticleEditor.h"
#include "ImGuiManager.h"
#include <stdio.h>
#include <fstream>
#include <sstream>
#include <iostream>
#include <cassert>
#include <filesystem>
#include <windows.h>
#include <cmath>

ParticleEditor* ParticleEditor::GetInstance()
{
	static ParticleEditor instance;
	return &instance;
}

void ParticleEditor::Initialize()
{

	demoParticles_.clear();

	isOpenFile_ = false;

	LoadPngFile();

	LoadAllData();

}

void ParticleEditor::Finalize()
{

	demoParticles_.clear();

}

void ParticleEditor::Update()
{

	demoParticles_.remove_if([](auto& particle) {

		if (not particle->isActive_) {
			return true;
		}

		return false;

		});

#ifdef _DEBUG

	ImGui::Begin("Particle Editor");

	//パーティクルデータ編集中
	if (isOpenFile_) {

		if (ImGui::Button("Save")) {
			Save();
		}

		if (ImGui::Button("Close")) {
			Close();
		}

		if (ImGui::BeginTabBar("Particles", ImGuiTabBarFlags_None)) {

			for (auto& particle : demoParticles_) {
				EditParticle(*particle);
			}

			ImGui::EndTabBar();
		}

	}
	//パーティクルデータの作成、ロード処理
	else {

		ImGui::InputText(".json", fileName_, sizeof(fileName_));

		if (ImGui::Button("Create")) {

			//ファイル名が空の場合スキップ
			if (!CheckIsEmpty(fileName_)) {
				Create();
			}
			else {
				MessageBox(nullptr, L"ファイル名を入力してください。", L"Particle Editor - Create", 0);
			}

		}

		if (ImGui::Button("Load")) {

			//ファイル名が空の場合スキップ
			if (!CheckIsEmpty(fileName_)) {
				Load();
			}
			else {
				MessageBox(nullptr, L"ファイル名を入力してください。", L"Particle Editor - Load", 0);
			}

		}

		ImGui::Separator();
		ImGui::Text("Particle List");

		for (int32_t i = 0; i < particleNames_.size(); i++) {

			if (ImGui::Button(particleNames_[i].c_str())) {

				for (int32_t k = 0; k < particleNames_[i].size(); k++) {
					fileName_[k] = particleNames_[i][k];
				}

				Load();
			}

		}

	}

	ImGui::End();

#endif // _DEBUG

	for (auto& particle : demoParticles_) {
		particle->Update();
	}

}

void ParticleEditor::Draw(Camera* camera)
{

	for (auto& particle : demoParticles_) {
		particle->Draw(camera);
	}

}

void ParticleEditor::EditParticle(Particle& particle)
{

#ifdef _DEBUG

	std::string treeName = particle.name_ + " ";

	if (ImGui::TreeNode(treeName.c_str())) {

		if (ImGui::InputText("Name", particle.name_.data(), 256)) {

		}

		if (ImGui::TreeNode("Speed")) {

			if (ImGui::DragFloat3("MinSpeed", &particle.minSpeed_.x, 0.01f)) {

			}

			if (ImGui::DragFloat3("MaxSpeed", &particle.maxSpeed_.x, 0.01f)) {

			}

			if (ImGui::DragFloat3("DeltaSpeed", &particle.changeSpeed_.x, 0.01f)) {

			}

			ImGui::TreePop();
		}

		if (ImGui::TreeNode("Scale")) {

			if (ImGui::DragFloat("MinScale", &particle.minScale_, 0.01f)) {

			}

			if (ImGui::DragFloat("MaxScale", &particle.maxScale_, 0.01f)) {

			}

			if (ImGui::DragFloat("DeltaScale", &particle.changeScale_, 0.01f)) {

			}

			ImGui::TreePop();
		}

		if (ImGui::TreeNode("SpawnPoint")) {

			if (ImGui::DragFloat3("MinSpawnPoint", &particle.minSpawnPoint_.x, 0.01f)) {

			}

			if (ImGui::DragFloat3("MaxSpawnPoint", &particle.maxSpawnPoint_.x, 0.01f)) {

			}

			ImGui::TreePop();
		}

		if (ImGui::TreeNode("LifeTime")) {

			if (ImGui::DragInt("MinTime", &particle.minLifeTime_, 1.0f, 0, 999)) {

			}

			if (ImGui::DragInt("MaxTime", &particle.maxLifeTime_, 1.0f, 0, 999)) {

			}

			if (ImGui::DragInt("AllTime", &particle.maxParticleLifeTime_, 1.0f, 0, 999)) {

			}

			ImGui::TreePop();
		}

		if (ImGui::TreeNode("Color")) {

			if (ImGui::DragFloat4("Start", &particle.startColor_.x, 0.01f, 0.0f, 1.0f)) {

			}

			if (ImGui::DragFloat4("End", &particle.endColor_.x, 0.01f, 0.0f, 1.0f)) {

			}

			ImGui::TreePop();
		}

		if (ImGui::DragInt("InstanceCount", &particle.instanceCount_, 0.5f, 0, Particle::kMaxParticle_)) {

		}

		if (ImGui::Checkbox("IsLoop", &particle.isLoop_)) {

		}

		if (ImGui::Button("Start")) {
			particle.particleLifeTime_ = particle.maxParticleLifeTime_;
		}

		if (ImGui::TreeNode("Textures")) {

			for (int32_t i = 0; i < textureNames_.size(); i++) {

				if (ImGui::Button(textureNames_[i].c_str())) {
					particle.SetTexture(textureMap_[textureNames_[i]]);
				}

			}

			ImGui::TreePop();
		}

		ImGui::TreePop();
	}

#endif // _DEBUG

}

void ParticleEditor::Load()
{

	//読み込むJSONファイルのフルパスを合成する
	std::string filePath = kDirectoryPath_ + fileName_ + ".json";
	//読み込み用ファイルストリーム
	std::ifstream ifs;
	//ファイルを読み込み用に開く
	ifs.open(filePath);

	//ファイルオープン失敗したら表示
	if (ifs.fail()) {
		MessageBox(nullptr, L"指定したファイルは存在しません。", L"Particle Editor - Load", 0);
		return;
	}

	nlohmann::json root;

	//json文字列からjsonのデータ構造に展開
	ifs >> root;
	//ファイルを閉じる
	ifs.close();
	//グループを検索
	nlohmann::json::iterator itGroup = root.find(kParticleName_);
	//未登録チェック
	if (itGroup == root.end()) {
		MessageBox(nullptr, L"ファイルの構造が正しくありません。", L"Particle Editor - Load", 0);
	}

	//保険
	assert(itGroup != root.end());

	//各アイテムについて
	for (nlohmann::json::iterator itItem = itGroup->begin(); itItem != itGroup->end(); ++itItem) {

		//アイテム名を取得
		const std::string& itemName = itItem.key();

		//グループを検索
		nlohmann::json::iterator itObject = itGroup->find(itemName);

		//未登録チェック
		if (itObject == itGroup->end()) {
			MessageBox(nullptr, L"ファイルの構造が正しくありません。", L"Particle Editor - Load", 0);
		}

		//保険
		assert(itObject != itGroup->end());

		//アイテム名がパーティクルデータだった場合、登録
		if (itemName == "ParticleData") {

			//新規作成
			std::shared_ptr<Particle> newParticle;
			newParticle = std::make_shared<Particle>();
			newParticle->Initialize();

			//パーティクルデータについて
			for (nlohmann::json::iterator itParticleData = itObject->begin(); itParticleData != itObject->end(); ++itParticleData) {

				//アイテム名を取得
				const std::string& dataName = itParticleData.key();

				//グループを検索
				nlohmann::json::iterator itData = itObject->find(dataName);

				//未登録チェック
				if (itData == itObject->end()) {
					MessageBox(nullptr, L"ファイルの構造が正しくありません。", L"Particle Editor - Load", 0);
				}

				//保険
				assert(itData != itObject->end());

				//Speedを登録
				if (dataName == "Speed") {

					for (nlohmann::json::iterator itDetailData = itData->begin(); itDetailData != itData->end(); ++itDetailData) {

						//アイテム名を取得
						const std::string& detailName = itDetailData.key();

						if (detailName == "Min") {
							newParticle->minSpeed_ = { itDetailData->at(0),itDetailData->at(1) ,itDetailData->at(2) };
						}
						if (detailName == "Max") {
							newParticle->maxSpeed_ = { itDetailData->at(0),itDetailData->at(1) ,itDetailData->at(2) };
						}
						if (detailName == "Delta") {
							newParticle->changeSpeed_ = { itDetailData->at(0),itDetailData->at(1) ,itDetailData->at(2) };
						}

					}

				}
				//SpawnPointを登録
				else if (dataName == "SpawnPoint") {

					for (nlohmann::json::iterator itDetailData = itData->begin(); itDetailData != itData->end(); ++itDetailData) {

						//アイテム名を取得
						const std::string& detailName = itDetailData.key();

						if (detailName == "Min") {
							newParticle->minSpawnPoint_ = { itDetailData->at(0),itDetailData->at(1) ,itDetailData->at(2) };
						}
						if (detailName == "Max") {
							newParticle->maxSpawnPoint_ = { itDetailData->at(0),itDetailData->at(1) ,itDetailData->at(2) };
						}

					}

				}
				else if (dataName == "Scale") {

					for (nlohmann::json::iterator itDetailData = itData->begin(); itDetailData != itData->end(); ++itDetailData) {

						//アイテム名を取得
						const std::string& detailName = itDetailData.key();

						if (detailName == "Min") {
							newParticle->minScale_ = itDetailData->get<float>();
						}
						if (detailName == "Max") {
							newParticle->maxScale_ = itDetailData->get<float>();
						}
						if (detailName == "Delta") {
							newParticle->changeScale_ = itDetailData->get<float>();
						}

					}

				}
				else if (dataName == "LifeTime") {

					for (nlohmann::json::iterator itDetailData = itData->begin(); itDetailData != itData->end(); ++itDetailData) {

						//アイテム名を取得
						const std::string& detailName = itDetailData.key();

						if (detailName == "Min") {
							newParticle->minLifeTime_ = itDetailData->get<int32_t>();
						}
						if (detailName == "Max") {
							newParticle->maxLifeTime_ = itDetailData->get<int32_t>();
						}
						if (detailName == "All") {
							newParticle->particleLifeTime_ = itDetailData->get<int32_t>();
						}

					}

				}
				else if (dataName == "Color") {

					for (nlohmann::json::iterator itDetailData = itData->begin(); itDetailData != itData->end(); ++itDetailData) {

						//アイテム名を取得
						const std::string& detailName = itDetailData.key();

						if (detailName == "Start") {
							newParticle->startColor_ = { itDetailData->at(0),itDetailData->at(1) ,itDetailData->at(2),itDetailData->at(3) };
						}
						if (detailName == "End") {
							newParticle->endColor_ = { itDetailData->at(0),itDetailData->at(1) ,itDetailData->at(2),itDetailData->at(3) };
						}

					}

				}
				else if (dataName == "Name") {
					newParticle->name_ = itData.value();
				}
				else if (dataName == "Instance") {
					newParticle->SetInstanceCount(itData->get<int32_t>());
				}
				else if (dataName == "IsLoop") {
					newParticle->isLoop_ = itData->get<bool>();
				}
				else if (dataName == "Texture") {
					newParticle->SetTexture(itData.value());
				}

			}

			demoParticles_.push_back(newParticle);

		}

	}

	isOpenFile_ = true;

}

void ParticleEditor::Save()
{

	nlohmann::json root;

	root = nlohmann::json::object();

	root[kParticleName_] = nlohmann::json::object();

	for (auto& particle : demoParticles_) {

		SaveParticle(root[kParticleName_], *particle);

	}

	//ディレクトリが無ければ作成する
	std::filesystem::path dir(kDirectoryPath_);
	if (!std::filesystem::exists(dir)) {
		std::filesystem::create_directory(dir);
	}

	//書き込むJSONファイルのフルパスを合成する
	std::string filePath = kDirectoryPath_ + fileName_ + ".json";
	//書き込み用ファイルストリーム
	std::ofstream ofs;
	//ファイルを書き込み用に開く
	ofs.open(filePath);

	//ファイルオープン失敗したら表示
	if (ofs.fail()) {
		MessageBox(nullptr, L"ファイルを開くのに失敗しました。", L"Particle Editor - Save", 0);
		return;
	}

	//ファイルにjson文字列を書き込む(インデント幅4)
	ofs << std::setw(4) << root << std::endl;
	//ファイルを閉じる
	ofs.close();

	MessageBox(nullptr, L"セーブしました。", L"Particle Editor - Save", 0);

	isSave_ = true;

}

void ParticleEditor::SaveParticle(nlohmann::json& jsonData, Particle& particle)
{

	jsonData["ParticleData"]["Name"] = particle.name_;
	jsonData["ParticleData"]["Speed"]["Min"] =
		nlohmann::json::array({ particle.minSpeed_.x, particle.minSpeed_.y, particle.minSpeed_.z });
	jsonData["ParticleData"]["Speed"]["Max"] =
		nlohmann::json::array({ particle.maxSpeed_.x, particle.maxSpeed_.y, particle.maxSpeed_.z });
	jsonData["ParticleData"]["Speed"]["Delta"] =
		nlohmann::json::array({ particle.changeSpeed_.x, particle.changeSpeed_.y, particle.changeSpeed_.z });
	jsonData["ParticleData"]["Scale"]["Min"] = particle.minScale_;
	jsonData["ParticleData"]["Scale"]["Max"] = particle.maxScale_;
	jsonData["ParticleData"]["Scale"]["Delta"] = particle.changeScale_;
	jsonData["ParticleData"]["SpawnPoint"]["Min"] =
		nlohmann::json::array({ particle.minSpawnPoint_.x, particle.minSpawnPoint_.y, particle.minSpawnPoint_.z });
	jsonData["ParticleData"]["SpawnPoint"]["Max"] =
		nlohmann::json::array({ particle.maxSpawnPoint_.x, particle.maxSpawnPoint_.y, particle.maxSpawnPoint_.z });
	jsonData["ParticleData"]["LifeTime"]["Min"] = particle.minLifeTime_;
	jsonData["ParticleData"]["LifeTime"]["Max"] = particle.maxLifeTime_;
	jsonData["ParticleData"]["LifeTime"]["All"] = particle.maxParticleLifeTime_;
	jsonData["ParticleData"]["Color"]["Start"] =
		nlohmann::json::array({ particle.startColor_.x,particle.startColor_.y ,particle.startColor_.z ,particle.startColor_.w });
	jsonData["ParticleData"]["Color"]["End"] =
		nlohmann::json::array({ particle.endColor_.x,particle.endColor_.y ,particle.endColor_.z ,particle.endColor_.w });
	jsonData["ParticleData"]["Instance"] = particle.instanceCount_;
	jsonData["ParticleData"]["IsLoop"] = particle.isLoop_;
	jsonData["ParticleData"]["Texture"] = particle.particle_->GetTexturePath();

}

void ParticleEditor::Create()
{

	//読み込むJSONファイルのフルパスを合成する
	std::string filePath = kDirectoryPath_ + fileName_ + ".json";

	std::filesystem::path path(filePath);

	//ファイルパスが存在するか確認
	if (std::filesystem::exists(path)) {

		if (MessageBox(nullptr, L"同名ファイルが既にあります。上書きしますか？", L"Particle Editor - Create", MB_OKCANCEL) == IDCANCEL) {

			return;

		}

	}

	//ディレクトリが無ければ作成する
	std::filesystem::path dir(kDirectoryPath_);
	if (!std::filesystem::exists(dir)) {
		std::filesystem::create_directory(dir);
	}

	std::ofstream newFile(filePath);

	//新規ファイル作成
	if (newFile.is_open()) {

		//jsonを読み込めるように空のシーン情報を記録
		nlohmann::json root;

		root = nlohmann::json::object();

		root[kParticleName_] = nlohmann::json::object();

		//ファイルにjson文字列を書き込む(インデント幅4)
		newFile << std::setw(4) << root << std::endl;
		//ファイルを閉じる
		newFile.close();
	}
	else {
		MessageBox(nullptr, L"ファイルを作成できませんでした。", L"Map Editor - Create", 0);
		return;
	}

	std::shared_ptr<Particle> newParticle;
	newParticle = std::make_shared<Particle>();
	newParticle->Initialize();
	newParticle->SetMinSpawnPoint({ -1.0f,-1.0f,-1.0f });
	newParticle->SetMaxSpawnPoint({ 1.0f,1.0f,1.0f });
	newParticle->SetMinLifeTime(60);
	newParticle->SetMaxLifeTime(60);
	newParticle->SetParticleLifeTime(60);
	newParticle->SetMinSpeed({ -0.01f,-0.01f,-0.01f });
	newParticle->SetMaxSpeed({ 0.01f,0.01f,0.01f });
	newParticle->SetMinScale(1.0f);
	newParticle->SetMaxScale(1.0f);
	newParticle->SetChangeScale(-0.05f);
	newParticle->SetColor({ 1.0f,1.0f,1.0f,1.0f });
	newParticle->SetIsLoop(true);
	demoParticles_.push_back(newParticle);

	isOpenFile_ = true;

}

void ParticleEditor::Close()
{

	if (not isSave_) {

		auto id = MessageBox(
			nullptr,
			L"ファイルが保存されていません。保存しますか？",
			L"Particle Editor - Close",
			MB_YESNOCANCEL);

		// OKの場合はセーブ
		if (id == IDYES) {
			Save();
		}
		// キャンセルした場合早期リターン
		else if (id == IDCANCEL) {
			return;
		}
		else if (id == IDNO) {
			// 何もしない
		}

	}

	std::memset(fileName_, 0, sizeof(fileName_));

	demoParticles_.clear();

	LoadAllData();

	isOpenFile_ = false;

}

bool ParticleEditor::CheckIsEmpty(const std::string& name) {

	if (name.empty()) {
		return true;
	}

	return false;

}

void ParticleEditor::LoadPngFile()
{

	textureNames_.clear();
	textureMap_.clear();

	//particleファイル内のpngファイル全検索
	std::filesystem::recursive_directory_iterator itr("./Resources/particle");

	//検索する拡張子
	std::string extension = ".png";

	//検索
	for (const auto& entry : itr) {

		if (std::filesystem::is_regular_file(entry.path()) and
			entry.path().extension() == extension) {
			std::string textureName = entry.path().stem().string();
			//名前と番号を紐づけ
			textureNumMap_[textureName] = int32_t(textureNames_.size());
			//最後尾に追加
			textureNames_.push_back(textureName);
			//名前とパスを紐づけ
			textureMap_[textureName] = entry.path().string();
		}

	}

}

void ParticleEditor::LoadAllData()
{

	particleNames_.clear();

	std::filesystem::recursive_directory_iterator itr("./Resources/ParticleData");

	//検索する拡張子
	std::string extension = ".json";

	//マップ全検索
	for (const auto& entry : itr) {

		if (std::filesystem::is_regular_file(entry.path()) and
			entry.path().extension() == extension) {
			std::string dataName = entry.path().stem().string();
			//最後尾に追加
			particleNames_.push_back(dataName);
		}

	}

}
