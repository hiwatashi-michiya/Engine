#include "Particle.h"
#include <random>
#include "Rand.h"

Particle::Particle()
{
}

Particle::~Particle()
{
}

void Particle::Initialize() {

	particle_.reset(Particle3D::Create("./Resources/particle/particle.obj", 128));
	particle_->SetInstanceCount(instanceCount_);


}

void Particle::Update() {

	//パーティクル全体の生存時間
	if (particleLifeTime_ > 0 and !isLoop_) {
		particleLifeTime_--;
	}

	for (uint32_t i = 0; i < instanceCount_; i++) {

		if ((isLoop_ or particleLifeTime_ > 0) and !particle_->isActive_[i]) {

			particle_->transforms_[i]->translate_ = RandomVector3(minSpawnPoint_, maxSpawnPoint_);
			particle_->transforms_[i]->rotateQuaternion_ = ConvertFromEuler(RandomVector3(-3.14f, 3.14f));
			particle_->transforms_[i]->scale_ = Vector3{ 1.0f,1.0f,1.0f } * RandomFloat(minScale_, maxScale_);

			particle_->velocities_[i] = RandomVector3(minSpeed_, maxSpeed_);
			particle_->lifeTimes_[i] = RandomInt(minLifeTime_, maxLifeTime_);
			particle_->colors_[i] = color_;

			particle_->isActive_[i] = true;

			break;

		}

	}

	for (uint32_t i = 0; i < instanceCount_; i++) {

		if (particle_->isActive_[i]) {

			particle_->transforms_[i]->translate_ += particle_->velocities_[i];
			particle_->transforms_[i]->scale_ += {changeScale_, changeScale_, changeScale_};

			if (particle_->transforms_[i]->scale_.x <= 0.0f or 
				particle_->transforms_[i]->scale_.y <= 0.0f or 
				particle_->transforms_[i]->scale_.z <= 0.0f or
				particle_->lifeTimes_[i] <= 0) {
				particle_->transforms_[i]->scale_ = Vector3::Zero();

				particle_->isActive_[i] = false;

			}

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
