#pragma once
#include "Engine/3d/Model.h"

class Stage
{
public:
	Stage();
	~Stage();

	void Initialize();

	void Update();

	void Draw();

private:

	std::unique_ptr<Model> model_;
	std::unique_ptr<Model> skydome_;

	Texture* tex_;
	Texture* tex2_;

};

