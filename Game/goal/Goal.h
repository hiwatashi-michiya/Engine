#pragma once
#include "Engine/3d/Model.h"
#include "Engine/math/Collision.h"
#include <memory>
#include "Engine/Tool/Particle.h"

class Goal
{
public:
	Goal();
	~Goal();

	enum Color {
		kNone,
		kRed,
		kGreen,
		kBlue,
		kYellow
	};

	void Initialize(const Color& color);

	void Update();

	void Draw(Camera* camera);

	void SetPosition(const Vector3& position) { 
		model_->position_ = position;
		particle_->startPosition_ = model_->position_;
		particle_->Reset();
	}

	const AABB& GetCollision() { return collision_; }

	const Color& GetColor() { return color_; }

	void SetIsGoal(bool flag) { isGoal_ = flag; }

private:

	std::unique_ptr<Model> model_;

	std::unique_ptr<Particle> particle_;

	AABB collision_{};

	Color color_;

	bool isGoal_ = false;

	Texture* tex1_;
	Texture* tex2_;

};
