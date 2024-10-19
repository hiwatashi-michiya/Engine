#pragma once
#include "Engine/3d/Model.h"
#include "Collision.h"
#include "Transform.h"
#include "GameObject.h"
#include "Collider.h"
#include "LineDrawer.h"
#include "Game/ColorSetter/ColorSetter.h"

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

	void SetPositionB(const Vector3& position) {
		transformB_->translate_ = position;
		colliderB_->collider_.center = transformB_->translate_;
		colliderB_->collider_.size = transformB_->scale_ / 2.0f;
	}

	void SetScaleB(const Vector3& scale) { transformB_->scale_ = scale; }

	void SetColor(GameColor::Color color) { color_ = color; }

	const Vector3& GetPosition() { return transform_->translate_; }

	const Vector3& GetScale() { return transform_->scale_; }

	const Vector3& GetPositionB() { return transformB_->translate_; }

	const Vector3& GetScaleB() { return transformB_->scale_; }

	BoxCollider* GetColliderA() { return colliderA_.get(); }

	BoxCollider* GetColliderB() { return colliderB_.get(); }

private:

	void OnCollisionA(Collider* collider);

	void OnCollisionB(Collider* collider);

private:

	//二つ分のモデル、当たり判定を使う

	std::unique_ptr<Model> modelA_;
	std::unique_ptr<Model> modelAWire_;

	std::unique_ptr<Model> modelB_;
	std::unique_ptr<Model> modelBWire_;

	std::unique_ptr<BoxCollider> colliderA_;

	std::unique_ptr<BoxCollider> colliderB_;

	std::unique_ptr<Transform> transformB_;

	std::unique_ptr<LineBox> lineBox_;

	std::unique_ptr<LineBox> lineBoxB_;

	bool isActiveWarp_ = true;

	bool isPreActiveWarp_ = true;

	int32_t countCoolTimer_ = 0;

	int32_t coolTime_ = 60;

	GameColor::Color color_ = GameColor::kWhite;

};
