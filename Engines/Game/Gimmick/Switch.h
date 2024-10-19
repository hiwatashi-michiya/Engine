#pragma once
#include "Model.h"
#include "GameObject.h"
#include "Collider.h"
#include "LineDrawer.h"
#include "Game/ColorSetter/ColorSetter.h"

class Switch : public GameObject
{
public:
	Switch();
	~Switch();

	void Initialize();

	void Update();

	void Draw(Camera* camera);

	void DrawLine(Camera* camera);

	void SetColor(GameColor::Color color) { color_ = color; }

private:

	void OnCollision(Collider* collider);

private:

	std::unique_ptr<Model> model_;
	std::unique_ptr<Model> modelWire_;

	std::unique_ptr<BoxCollider> collider_;

	std::unique_ptr<LineBox> lineBox_;

	GameColor::Color color_ = GameColor::kWhite;

	int32_t coolTime_ = 30;

	int32_t countTimer_ = 0;

};


