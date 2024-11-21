#pragma once
#include "Model.h"
#include "GameObject.h"
#include "Collider.h"
#include <memory>
#include "LineDrawer.h"
#include "Game/player/Player.h"
#include "Particle3D.h"

class Goal : public GameObject
{
public:
	Goal();
	~Goal();

	void Initialize();

	void Update();

	void Draw(Camera* camera);

	void SetPosition(const Vector3& position) {
		transform_->translate_ = position;
		collider_->collider_.center = transform_->translate_;
		collider_->collider_.size = transform_->scale_;
	}

	void SetScale(const Vector3& scale) { transform_->scale_ = scale; }

	const Vector3& GetPosition() { return transform_->translate_; }

	const Vector3& GetScale() { return transform_->scale_; }

	BoxCollider* GetCollider() { return collider_.get(); }

	void SetPlayer(Player* player) { player_ = player; }

private:

	Player* player_ = nullptr;

	std::unique_ptr<Model> model_;

	std::unique_ptr<Particle3D> particle_;

	std::unique_ptr<BoxCollider> collider_;

	std::unique_ptr<LineBox> lineBox_;

	bool isActiveEffect_ = false;

	int32_t particleCount_ = 32;

};


