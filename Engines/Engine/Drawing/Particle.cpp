#include "Particle.h"
#include <random>
#include "Rand.h"
#include <json.hpp>
#include <iostream>
#include <fstream>
#include "ParticleEditor.h"

Particle::Particle()
{
}

Particle::~Particle()
{
}

void Particle::Initialize() {

	particle_.reset(Particle3D::Create("./Resources/particle/plane.obj", kMaxParticle_));
	particle_->SetInstanceCount(instanceCount_);

}

void Particle::Update() {

	//パーティクル全体の生存時間
	if (particleLifeTime_ > 0 and not isLoop_) {
		particleLifeTime_--;
	}

	for (int32_t i = 0; i < instanceCount_; i++) {
		//非アクティブのパーティクルを出現させる
		if ((isLoop_ or particleLifeTime_ > 0) and not particle_->isActive_[i]) {

			particle_->transforms_[i]->translate_ = RandomVector3(spawnPoint_ + minSpawnPoint_, spawnPoint_ + maxSpawnPoint_);
			//追従対象がいたら対象の座標分加算
			if (targetPoint_) {
				particle_->transforms_[i]->translate_ += *targetPoint_;
			}

			particle_->transforms_[i]->rotateQuaternion_ = ConvertFromEuler(RandomVector3(-3.14f, 3.14f));
			particle_->transforms_[i]->scale_ = Vector3{ 1.0f,1.0f,1.0f } * RandomFloat(minScale_, maxScale_);

			particle_->velocities_[i] = RandomVector3(minSpeed_, maxSpeed_);
			particle_->lifeTimes_[i] = RandomInt(minLifeTime_, maxLifeTime_);
			particle_->colors_[i] = RandomVector4(minStartColor_, maxStartColor_);

			particle_->isActive_[i] = true;

			break;

		}

	}

	for (int32_t i = 0; i < instanceCount_; i++) {

		//アクティブ状態のパーティクルを更新
		if (particle_->isActive_[i]) {

			particle_->transforms_[i]->translate_ += particle_->velocities_[i];
			particle_->transforms_[i]->scale_ += {changeScale_, changeScale_, changeScale_};
			/*particle_->colors_[i] = Lerp(endColor_, minStartColor_, float(particle_->transforms_[i]->scale_.x) / maxScale_);*/

			particle_->velocities_[i] += changeSpeed_;

			if (particle_->transforms_[i]->scale_.x <= 0.0f or 
				particle_->transforms_[i]->scale_.y <= 0.0f or 
				particle_->transforms_[i]->scale_.z <= 0.0f or
				particle_->lifeTimes_[i] <= 0) {
				particle_->transforms_[i]->scale_ = Vector3::Zero();

				particle_->isActive_[i] = false;

			}

			particle_->lifeTimes_[i]--;

		}

	}

}

void Particle::Draw(Camera* camera) {

	particle_->Draw(camera);

}

bool Particle::IsEnd() {

	if (particleLifeTime_ <= 0 and !particle_->IsAnyActive() and !isLoop_) {
		return true;
	}

	return false;

}

void Particle::Load(const std::string& filePath)
{

	//読み込み用ファイルストリーム
	std::ifstream ifs;
	//ファイルを読み込み用に開く
	ifs.open(filePath);

	//ファイルオープン失敗したら表示
	if (ifs.fail()) {
		MessageBox(nullptr, L"指定したファイルは存在しません。", L"Particle - Load", 0);
		return;
	}

	nlohmann::json root;

	//json文字列からjsonのデータ構造に展開
	ifs >> root;
	//ファイルを閉じる
	ifs.close();
	//グループを検索
	nlohmann::json::iterator itGroup = root.find(ParticleEditor::kParticleName_);
	//未登録チェック
	if (itGroup == root.end()) {
		MessageBox(nullptr, L"ファイルの構造が正しくありません。", L"Particle - Load", 0);
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
			MessageBox(nullptr, L"ファイルの構造が正しくありません。", L"Particle - Load", 0);
		}

		//保険
		assert(itObject != itGroup->end());

		//アイテム名がパーティクルデータだった場合、登録
		if (itemName == "ParticleData") {

			//パーティクルデータについて
			for (nlohmann::json::iterator itParticleData = itObject->begin(); itParticleData != itObject->end(); ++itParticleData) {

				//アイテム名を取得
				const std::string& dataName = itParticleData.key();

				//グループを検索
				nlohmann::json::iterator itData = itObject->find(dataName);

				//未登録チェック
				if (itData == itObject->end()) {
					MessageBox(nullptr, L"ファイルの構造が正しくありません。", L"Particle - Load", 0);
				}

				//保険
				assert(itData != itObject->end());

				//Speedを登録
				if (dataName == "Speed") {

					for (nlohmann::json::iterator itDetailData = itData->begin(); itDetailData != itData->end(); ++itDetailData) {

						//アイテム名を取得
						const std::string& detailName = itDetailData.key();

						if (detailName == "Min") {
							minSpeed_ = { itDetailData->at(0),itDetailData->at(1) ,itDetailData->at(2) };
						}
						if (detailName == "Max") {
							maxSpeed_ = { itDetailData->at(0),itDetailData->at(1) ,itDetailData->at(2) };
						}
						if (detailName == "Delta") {
							changeSpeed_ = { itDetailData->at(0),itDetailData->at(1) ,itDetailData->at(2) };
						}

					}

				}
				//SpawnPointを登録
				else if (dataName == "SpawnPoint") {

					for (nlohmann::json::iterator itDetailData = itData->begin(); itDetailData != itData->end(); ++itDetailData) {

						//アイテム名を取得
						const std::string& detailName = itDetailData.key();

						if (detailName == "Min") {
							minSpawnPoint_ = { itDetailData->at(0),itDetailData->at(1) ,itDetailData->at(2) };
						}
						if (detailName == "Max") {
							maxSpawnPoint_ = { itDetailData->at(0),itDetailData->at(1) ,itDetailData->at(2) };
						}

					}

				}
				else if (dataName == "Scale") {

					for (nlohmann::json::iterator itDetailData = itData->begin(); itDetailData != itData->end(); ++itDetailData) {

						//アイテム名を取得
						const std::string& detailName = itDetailData.key();

						if (detailName == "Min") {
							minScale_ = itDetailData->get<float>();
						}
						if (detailName == "Max") {
							maxScale_ = itDetailData->get<float>();
						}
						if (detailName == "Delta") {
							changeScale_ = itDetailData->get<float>();
						}

					}

				}
				else if (dataName == "LifeTime") {

					for (nlohmann::json::iterator itDetailData = itData->begin(); itDetailData != itData->end(); ++itDetailData) {

						//アイテム名を取得
						const std::string& detailName = itDetailData.key();

						if (detailName == "Min") {
							minLifeTime_ = itDetailData->get<int32_t>();
						}
						if (detailName == "Max") {
							maxLifeTime_ = itDetailData->get<int32_t>();
						}
						if (detailName == "All") {
							particleLifeTime_ = itDetailData->get<int32_t>();
						}

					}

				}
				else if (dataName == "Color") {

					for (nlohmann::json::iterator itDetailData = itData->begin(); itDetailData != itData->end(); ++itDetailData) {

						//アイテム名を取得
						const std::string& detailName = itDetailData.key();

						if (detailName == "Start") {
							minStartColor_ = { itDetailData->at(0),itDetailData->at(1) ,itDetailData->at(2),itDetailData->at(3) };
						}
						if (detailName == "MinStart") {
							minStartColor_ = { itDetailData->at(0),itDetailData->at(1) ,itDetailData->at(2),itDetailData->at(3) };
						}
						if (detailName == "MaxStart") {
							maxStartColor_ = { itDetailData->at(0),itDetailData->at(1) ,itDetailData->at(2),itDetailData->at(3) };
						}
						if (detailName == "End") {
							endColor_ = { itDetailData->at(0),itDetailData->at(1) ,itDetailData->at(2),itDetailData->at(3) };
						}

					}

				}
				else if (dataName == "Name") {
					name_ = itData.value();
				}
				else if (dataName == "Instance") {
					SetInstanceCount(itData->get<int32_t>());
				}
				else if (dataName == "IsLoop") {
					isLoop_ = itData->get<bool>();
				}
				else if (dataName == "Texture") {
					particle_->SetTexture(itData.value());
				}

			}

		}

	}

}
