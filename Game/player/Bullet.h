#pragma once
#include "Engine/3d/Model.h"
#include <memory>
#include "Engine/math/Collision.h"

class Player;

class Bullet
{
public:
	Bullet();
	~Bullet();

	void Initialize(const Vector3& position);

	void Update();

	void Draw(Camera* camera);

	bool GetIsDead() const { return isDead_; }

	void SetIsDead(bool flag) { isDead_ = flag; }

	bool GetIsShot() const { return isShot_; }

	void Shot(const Vector3& position);

	void SetPosition(const Vector3& position) { model_->position_ = position; }

	void Charge(const Vector3& position);

	const Sphere& GetCollision() { return collision_; }

private:

	Player* player_ = nullptr;

	std::unique_ptr<Model> model_;

	bool isDead_ = false;

	bool isShot_ = false;

	Vector3 velocity_{};

	//生存時間
	int32_t liveTime_;

	uint32_t maxLiveTime_ = 120;

	//当たり判定(球)
	Sphere collision_{};

};
