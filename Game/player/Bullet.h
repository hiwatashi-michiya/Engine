#pragma once
#include "Engine/3d/Model.h"
#include <memory>

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

	bool GetIsShot() const { return isShot_; }

	void Shot(const Vector3& position);

	void SetPosition(const Vector3& position) { model_->position_ = position; }

	void Charge(const Vector3& position);

private:

	Player* player_ = nullptr;

	std::unique_ptr<Model> model_;

	bool isDead_ = false;

	bool isShot_ = false;

	Vector3 velocity_{};

	//生存時間
	int32_t liveTime_;

	uint32_t maxLiveTime_ = 120;

};
