#pragma once
#include "Engine/3d/Model.h"
#include "Engine/math/Collision.h"
#include "Transform.h"

class Block
{
public:
	Block();
	~Block();

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
		transform_->translate_ = position;
		collision_.max = transform_->translate_ + transform_->scale_ / 2.0f;
		collision_.min = transform_->translate_ - transform_->scale_ / 2.0f;
	}

	void SetScale(const Vector3& scale) { transform_->scale_ = scale; }

	const Vector3& GetPosition() { return transform_->translate_; }

	void SetIsRock(bool flag) { isRock_ = flag; }

	bool GetIsRock() { return isRock_; }

	const AABB& GetCollision() { return collision_; }

	const Color& GetColor() { return color_; }

private:

	std::unique_ptr<Model> model_;

	std::unique_ptr<Transform> transform_;

	AABB collision_;

	bool isRock_ = false;

	Color color_;

	float pLightIntencity_ = 0.5f;

	int32_t lightCount_ = 0;

	int32_t setCount_;

};
