#include "ParticleManager.h"

ParticleManager* ParticleManager::GetInstance() {
	static ParticleManager instance;
	return &instance;
}

void ParticleManager::Initialize() {

	particles_.clear();

}

void ParticleManager::Update() {
	//終了したパーティクルを取り出す
	particles_.remove_if([](std::unique_ptr<Particle>& particle) {

		if (particle->IsEnd()) {
			return true;
		}

		return false;

		});
	//パーティクル更新
	for (std::unique_ptr<Particle>& particle : particles_) {
		particle->Update();
	}

}

void ParticleManager::AddParticle(std::unique_ptr<Particle>& particle) {

	particles_.push_back(std::move(particle));

}

void ParticleManager::Draw(Camera* camera) {

	for (std::unique_ptr<Particle>& particle : particles_) {
		particle->Draw(camera);
	}

}
