#pragma once
#include "Engine/3d/Particle3D.h"
#include <memory>
#include <string>

class Particle
{
public:
	Particle();
	~Particle();

	//3dか2dか
	enum Type 
	{
		k3D,
	};

	void Initialize(const Type& type);

	void Reset();

	void AddParticle(const std::string& name, Texture* texture, uint32_t instanceCount);

	void Update();

	void Draw(Camera* camera);

	Vector3 startPosition_{};

private:

	std::vector<std::shared_ptr<Particle3D>> particles3D_;

	Type type_;

};
