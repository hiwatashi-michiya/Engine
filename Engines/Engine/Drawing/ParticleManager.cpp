#include "ParticleManager.h"

ParticleManager* ParticleManager::GetInstance() {
	static ParticleManager instance;
	return &instance;
}

void ParticleManager::Initialize() {

	for (int32_t i = 0; i < kMaxParticle_; i++) {

		if (not particles_[i]) {
			particles_[i] = std::make_unique<Particle>();
			particles_[i]->Initialize();
		}
		particles_[i]->SetDefault();
		isUsed_[i] = false;
	}

}

void ParticleManager::Finalize()
{

	for (int32_t i = 0; i < kMaxParticle_; i++) {

		if (particles_[i]) {
			particles_[i].release();
		}
	}

}

void ParticleManager::Update() {
	
	//パーティクル更新
	for (int32_t i = 0; i < kMaxParticle_; i++) {

		particles_[i]->Update();
		//パーティクルが終了した時に使用フラグを降ろす
		if (particles_[i]->IsEnd()) {
			isUsed_[i] = false;
		}

	}

}

void ParticleManager::Draw(Camera* camera) {

	for (int32_t i = 0; i < kMaxParticle_; i++) {

		if (isUsed_[i]) {
			particles_[i]->Draw(camera);
		}

	}

}

void ParticleManager::Reset()
{

	for (int32_t i = 0; i < kMaxParticle_; i++) {

		particles_[i]->SetDefault();
		isUsed_[i] = false;

	}

}

bool ParticleManager::GetUnUsedParticle(Particle*& particle)
{

	//全てのパーティクルから検索し、未使用のパーティクルのポインタを渡す
	for (int32_t i = 0; i < kMaxParticle_; i++) {

		if (not isUsed_[i]) {
			particle = particles_[i].get();
			isUsed_[i] = true;
			//取得出来たらtrueを返す
			return true;
		}

	}

	//取得できなかったのでnullptrを渡す
	particle = nullptr;
	//取得できなかったらfalseを返す
	return false;
}
