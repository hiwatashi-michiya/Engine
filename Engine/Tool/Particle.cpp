#include "Particle.h"
#include "Engine/math/Rand.h"

Particle::Particle()
{
}

Particle::~Particle()
{
}

void Particle::Initialize(const Dimension& dimension, const Type& type) {

	dimension_ = dimension;
	type_ = type;

}

void Particle::Reset() {

	for (auto& particle : particles3D_) {

		for (uint32_t i = 0; i < particle->instanceCount_; i++) {

			switch (type_)
			{
			case Particle::kCircle:
				particle->velocities_[i] = { float((rand() % 40 - 20) * 0.02f + 0.01f), float((rand() % 40 - 20) * 0.02f + 0.01f),
				float((rand() % 40 - 20) * 0.02f + 0.01f) };

				if (particle->velocities_[i].x == 0.0f && particle->velocities_[i].y == 0.0f &&
					particle->velocities_[i].z == 0.0f) {
					particle->velocities_[i].z = 0.2f;
				}

				particle->positions_[i] = startPosition_;

				break;
			case Particle::kUp:

				particle->velocities_[i] = { 0.0f,0.0f,float((rand() % 40 - 20) * 0.02f + 0.01f) };

				if (particle->velocities_[i].z == 0.0f) {
					particle->velocities_[i].z = 0.2f;
				}

				particle->positions_[i] = startPosition_ + Vector3{ float((rand() % 40 - 20) * 0.02f + 0.01f), float((rand() % 40 - 20) * 0.02f + 0.01f),
					float((rand() % 20 - 10) * 0.02f + 0.01f) };

				break;
			default:
				break;
			}
		}

		particle->mesh_->material_->constMap_->color = color_;

	}

}

void Particle::Update() {

	for (auto& particle : particles3D_) {
		
		for (uint32_t i = 0; i < particle->instanceCount_; i++) {

			switch (type_)
			{
			case Particle::kCircle:

				if (particle->mesh_->material_->constMap_->color.w > 0.0f) {
					particle->positions_[i] += particle->velocities_[i];
				}

				break;
			case Particle::kUp:

				particle->positions_[i] += particle->velocities_[i];

				if (particle->positions_[i].z >= endZ_) {

					particle->velocities_[i] = { 0.0f,0.0f,float((rand() % 40 - 20) * 0.02f + 0.01f) };

					if (particle->velocities_[i].z == 0.0f) {
						particle->velocities_[i].z = 0.2f;
					}

					particle->positions_[i] = startPosition_ + Vector3{ float((rand() % 40 - 20) * 0.02f + 0.01f), float((rand() % 40 - 20) * 0.02f + 0.01f),
						float((rand() % 20 - 10) * 0.02f + 0.01f) };

				}

				break;
			default:
				break;
			}

		}

		switch (type_)
		{
		case Particle::kCircle:

			if (particle->mesh_->material_->constMap_->color.w > 0.0f) {
				particle->mesh_->material_->constMap_->color.w -= 0.1f;
			}

			break;
		case Particle::kUp:
			break;
		default:
			break;
		}

	}

}

void Particle::Draw(Camera* camera) {

	for (auto& particle : particles3D_) {

		if (particle->mesh_->material_->constMap_->color.w > 0.0f) {
			particle->Draw(camera);
		}

	}

}

void Particle::AddParticle(const std::string& name, Texture* texture, uint32_t instanceCount) {

	std::shared_ptr<Particle3D> newParticle;
	newParticle.reset(Particle3D::Create(name, instanceCount));
	newParticle->SetTexture(texture);
	particles3D_.push_back(newParticle);

}
