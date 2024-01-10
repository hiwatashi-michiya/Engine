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
	enum Dimension 
	{
		k3D,
	};

	enum Type {
		kCircle,
		kUp,
	};

	void Initialize(const Dimension& dimension, const Type& type);

	void Reset();

	void AddParticle(const std::string& name, Texture* texture, uint32_t instanceCount);

	void Update();

	void Draw(Camera* camera);

	void SetColor(const Vector4& color, uint32_t num = 0) { 

		for (uint32_t i = 0; auto & particle : particles3D_) {

			if (i == num) {
				colors_[i] = color;
			}

			i++;

		}

	}

	void SetIsStart(bool flag) { isStart_ = flag; }

	Vector3 startPosition_{};

	//終点座標、半径
	float endX_ = 0.0f;
	float endY_ = 0.0f;
	float endZ_ = 0.0f;
	float endRadius_ = 10.0f;

private:

	std::vector<std::shared_ptr<Particle3D>> particles3D_;
	std::vector<Vector4> colors_;

	Dimension dimension_;

	Type type_;

	Vector4 color_{ 1.0f,1.0f,1.0f,1.0f };

	bool isStart_ = false;

};
