#pragma once
#include "Model.h"
#include "GameObject.h"
#include "Collider.h"

class Switch : public GameObject
{
public:
	Switch();
	~Switch();

	void Initialize();

	void Update();

	void Draw(Camera* camera);

private:

	std::unique_ptr<Model> model_;

	std::unique_ptr<BoxCollider> collider_;

};


