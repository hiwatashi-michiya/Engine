#pragma once
#include "Engine/3d/Model.h"
#include "Collision.h"
#include "Transform.h"
#include "GameObject.h"
#include "Collider.h"
#include "LineDrawer.h"
#include "Vector3.h"
#include "Game/ColorSetter/ColorSetter.h"

class GhostBox : public GameObject
{
public:
	GhostBox();
	~GhostBox();

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

	void SetColor(GameColor::Color color) { color_ = color; }

private:

	void OnCollision(Collider* collider);

private:

	std::unique_ptr<Model> model_;
	std::unique_ptr<Model> modelWire_;

	std::unique_ptr<BoxCollider> collider_;

	std::unique_ptr<LineBox> lineBox_;

	Vector3 preTranslate_{};

	GameColor::Color color_ = GameColor::kWhite;

};
