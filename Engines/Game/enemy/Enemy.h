#pragma once
#include "Engine/3d/Model.h"
#include "Collision.h"
#include "Transform.h"
#include "GameObject.h"
#include "Collider.h"
#include "LineDrawer.h"
#include "Vector3.h"
#include "Game/ColorSetter/ColorSetter.h"

class Enemy : public GameObject
{
public:
	Enemy();
	~Enemy();

	void Initialize();

	void Update();

	void Draw(Camera* camera);

	void DrawLine(Camera* camera);

	void SetColor(GameColor::Color color) { color_ = color; }

	bool GetIsDead() const { return isDead_; }

private:

	void OnCollision(Collider* collider);

private:

	std::unique_ptr<Model> model_;
	std::unique_ptr<Model> modelWire_;

	std::unique_ptr<LineBox> lineBox_;

	std::unique_ptr<BoxCollider> collider_;

	bool isDead_ = false;

	GameColor::Color color_ = GameColor::kWhite;

};


