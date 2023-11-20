#pragma once
#include "Engine/3d/Model.h"
#include "Engine/3d/WorldTransform.h"
#include <memory>
#include "Engine/input/Input.h"
#include "Engine/2d/Sprite.h"

class Player
{
public:
	Player();
	~Player();

	void Initialize();

	void Update();

	void Draw2D();

	void MoveLeft();

	void MoveRight();

private:

	Input* input_ = nullptr;

	std::unique_ptr<Sprite> sprite_;

	Texture tex_;

	Vector3 velocity_{};

	float speed_;

};


