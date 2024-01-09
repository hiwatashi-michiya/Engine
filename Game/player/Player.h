#pragma once
#include "Engine/3d/Model.h"
#include <memory>
#include "Engine/input/Input.h"
#include "Engine/math/Quaternion.h"
#include "Engine/math/Collision.h"
#include "Engine/Tool/Particle.h"

class Player
{
public:
	Player();
	~Player();

	void Initialize();

	void Update();

	void Draw(Camera* camera);

	void DrawParticle(Camera* camera);

	void SetPosition(const Vector3& position) { model_->position_ = position; }

	const AABB& GetCollision() { return collision_; }

	enum Move {
		kUp,
		kDown,
		kRight,
		kLeft
	};

	Move move_ = kDown;

private:

private:

	Input* input_ = nullptr;

	std::unique_ptr<Model> model_;

	std::unique_ptr<Particle> particle_;

	Vector3 velocity_{};

	bool canJump_ = true;

	Quaternion rotation_;

	AABB collision_{};

	bool isMove_ = false;

	const float moveVal_ = 2.0f;

	const uint32_t kMaxCoolTime_ = 10;

	uint32_t moveCoolTimer_ = 0;

	Texture* tex_;

};

