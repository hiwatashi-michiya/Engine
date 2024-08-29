#pragma once
#include "Particle3D.h"
#include "Vector3.h"
#include <memory>
#include <list>
#include "Particle.h"

class ParticleManager
{
public:
	
	static ParticleManager* GetInstance();

	void Initialize();

	void Update();

	void Draw(Camera* camera);

	void AddParticle(std::shared_ptr<Particle> particle);

private:

	std::list<std::shared_ptr<Particle>> particles_;

private:

	ParticleManager() = default;
	~ParticleManager() = default;
	ParticleManager(const ParticleManager&) = delete;
	const ParticleManager& operator=(const ParticleManager&) = delete;

};


