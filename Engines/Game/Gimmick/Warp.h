#pragma once
#include "Engine/3d/Model.h"
#include "Collision.h"
#include "Transform.h"
#include "GameObject.h"
#include "Collider.h"
#include "LineDrawer.h"

class Warp : public GameObject
{
public:
	Warp();
	~Warp();

	void Initialize();

	void Update();

	void Draw(Camera* camera);

	void DrawLine(Camera* camera);

	void SetPosition(const Vector3& position) {
		transform_->translate_ = position;
		colliderA_->collider_.center = transform_->translate_;
		colliderA_->collider_.size = transform_->scale_ / 2.0f;
	}

	void SetScale(const Vector3& scale) { transform_->scale_ = scale; }

	const Vector3& GetPosition() { return transform_->translate_; }

	const Vector3& GetScale() { return transform_->scale_; }

	BoxCollider* GetColliderA() { return colliderA_.get(); }

	BoxCollider* GetColliderB() { return colliderB_.get(); }

private:

	void OnCollision(Collider* collider);

private:

	//二つ分のモデル、当たり判定を使う

	std::unique_ptr<Model> modelA_;

	std::unique_ptr<Model> modelB_;

	std::unique_ptr<BoxCollider> colliderA_;

	std::unique_ptr<BoxCollider> colliderB_;

	std::unique_ptr<Transform> transformB_;

	std::unique_ptr<LineBox> lineBox_;

	Vector3 preTranslate_{};

};
