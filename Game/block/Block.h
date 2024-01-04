#pragma once
#include "Engine/3d/Model.h"
#include "Engine/math/Collision.h"

class Block
{
public:
	Block();
	~Block();

	void Initialize();

	void Update();

	void Draw(Camera* camera);

private:

	std::unique_ptr<Model> model_;

	AABB collision_;

};
