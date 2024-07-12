#pragma once
#include "Model.h"
#include "GameObject.h"
#include "Collider.h"
#include <memory>
#include "LineDrawer.h"

class Goal : public GameObject
{
public:
	Goal();
	~Goal();

private:

	std::unique_ptr<Model> model_;

	std::unique_ptr<Collider> collider_;

	std::unique_ptr<LineBox> lineBox_;

};


