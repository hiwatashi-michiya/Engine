#pragma once
#include "Engine/3d/Model.h"
#include <memory>
#include "Engine/input/Input.h"
#include "Engine/math/Quaternion.h"

class Player
{
public:
	Player();
	~Player();

	void Initialize();

	void Update();

	void Draw(Camera* camera);

	void SetPosition(Vector3& position) { model_->position_ = position; }

private:

	Input* input_ = nullptr;

	std::unique_ptr<Model> model_;

	Vector3 velocity_{};

	bool canJump_ = true;

	Quaternion rotation_;

};

