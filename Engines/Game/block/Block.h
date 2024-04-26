#pragma once
#include "Engine/3d/Model.h"
#include "Engine/math/Collision.h"
#include "Transform.h"

class Block
{
public:
	Block();
	~Block();

	void Initialize();

	void Update();

	void Draw(Camera* camera);

	void SetPosition(const Vector3& position) { 
		transform_->translate_ = position;
		collision_.max = transform_->translate_ + transform_->scale_ / 2.0f;
		collision_.min = transform_->translate_ - transform_->scale_ / 2.0f;
	}

	void SetScale(const Vector3& scale) { transform_->scale_ = scale; }

	const Vector3& GetPosition() { return transform_->translate_; }

	const AABB& GetCollision() { return collision_; }

private:

	std::unique_ptr<Model> model_;

	std::unique_ptr<Transform> transform_;

	AABB collision_;

	float pLightIntensity_ = 0.5f;

	int32_t lightCount_ = 0;

	int32_t setCount_;

};
