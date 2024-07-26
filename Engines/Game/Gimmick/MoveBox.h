#pragma once
#include "Engine/3d/Model.h"
#include "Collision.h"
#include "Transform.h"
#include "GameObject.h"
#include "Collider.h"
#include "LineDrawer.h"

class MoveBox : public GameObject
{
public:
	MoveBox();
	~MoveBox();

	void Initialize();

	void Update();

	void Draw(Camera* camera);

	void DrawLine(Camera* camera);

	void SetPosition(const Vector3& position) {
		transform_->translate_ = position;
		collider_->collider_.center = transform_->translate_;
		collider_->collider_.size = transform_->scale_ / 2.0f;
	}

	void SetScale(const Vector3& scale) { transform_->scale_ = scale; }

	const Vector3& GetPosition() { return transform_->translate_; }

	const Vector3& GetScale() { return transform_->scale_; }

	BoxCollider* GetCollider() { return collider_.get(); }

	void SetColor(int32_t color) { colorNumber_ = color; }

private:

	void OnCollision(Collider* collider);

private:

	std::unique_ptr<Model> model_;

	std::unique_ptr<BoxCollider> collider_;

	std::unique_ptr<LineBox> lineBox_;

	Vector3 preTranslate_{};

	int32_t colorNumber_ = 0;

	Texture* circleTex_;
	Texture* crossTex_;

};
