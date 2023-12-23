#include "Particle.h"
#include "Engine/math/Rand.h"

Particle::Particle()
{
}

Particle::~Particle()
{
}

void Particle::Initialize(const Type& type) {

	type_ = type;

}

void Particle::Reset() {

	for (auto& particle : particles3D_) {

		for (uint32_t i = 0; i < particle->instanceCount_; i++) {
			particle->velocities_[i] = { float((rand() % 40 - 20) * 0.1f), float((rand() % 40 - 20) * 0.1f) , float((rand() % 40 - 20) * 0.1f) };
			particle->positions_[i] = startPosition_;
		}

	}

}

void Particle::Update() {

	for (auto& particle : particles3D_) {
		
		for (uint32_t i = 0; i < particle->instanceCount_; i++) {
			particle->positions_[i] += particle->velocities_[i];
		}

	}

}

void Particle::Draw(Camera* camera) {

	for (auto& particle : particles3D_) {
		particle->Draw(camera);
	}



}

void Particle::AddParticle(const std::string& name, Texture* texture, uint32_t instanceCount) {

	std::shared_ptr<Particle3D> newParticle;
	newParticle.reset(Particle3D::Create(name, instanceCount));
	newParticle->SetTexture(texture);
	particles3D_.push_back(newParticle);

}
