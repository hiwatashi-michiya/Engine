#pragma once
#include "Engine/3d/Model.h"
#include <memory>

class Player;

class Bullet
{
public:
	Bullet();
	~Bullet();

	void Initialize(Player* player);

	void Update();

	void Draw();

	bool GetIsDead() const { return isDead_; }

	bool GetIsShot() const { return isShot_; }

	void Shot();

	void SetPosition(const Vector3& position) { model_->position_ = position; }

private:

	Player* player_ = nullptr;

	std::unique_ptr<Model> model_;

	bool isDead_ = false;

	bool isShot_ = false;

	Vector3 velocity_{};

	//生存時間
	int32_t liveTime_;

	uint32_t maxLiveTime_ = 60;

};
