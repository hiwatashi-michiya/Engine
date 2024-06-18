#pragma once
#include "Engine/3d/Model.h"
#include "Collision.h"
#include "Transform.h"
#include "GameObject.h"
#include "Collider.h"

class Block : public GameObject
{
public:
	Block();
	~Block();

	void Initialize();

	void Update();

	void Draw(Camera* camera);

	void SetPosition(const Vector3& position) { 
		transform_->translate_ = position;
		collider_->collider_.center = transform_->translate_;
		collider_->collider_.size = transform_->scale_ / 2.0f;
	}

	void SetScale(const Vector3& scale) { transform_->scale_ = scale; }

	const Vector3& GetPosition() { return transform_->translate_; }

	const Vector3& GetScale() { return transform_->scale_; }

	BoxCollider* GetCollider() { return collider_.get(); }

private:

	std::unique_ptr<Model> model_;

	std::unique_ptr<BoxCollider> collider_;

	float pLightIntensity_ = 0.5f;

	int32_t lightCount_ = 0;

	int32_t setCount_;

};
