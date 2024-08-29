#include "ParticleManager.h"

ParticleManager* ParticleManager::GetInstance() {
	static ParticleManager instance;
	return &instance;
}

void ParticleManager::Initialize() {

	particles_.clear();

}

void ParticleManager::Update() {

	particles_.remove_if([](std::shared_ptr<Particle> particle) {

		if (particle->IsEnd()) {
			return true;
		}

		return false;

		});

	for (auto& particle : particles_) {
		particle->Update();
	}

}

void ParticleManager::AddParticle(std::shared_ptr<Particle> particle) {

	particles_.push_back(particle);

}

void ParticleManager::Draw(Camera* camera) {

	for (auto& particle : particles_) {
		particle->Draw(camera);
	}

}
