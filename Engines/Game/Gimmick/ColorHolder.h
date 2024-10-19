#pragma once
#include "Model.h"
#include "GameObject.h"
#include "Collider.h"
#include "LineDrawer.h"
#include "Game/ColorSetter/ColorSetter.h"

class ColorHolder : public GameObject
{
public:
	ColorHolder();
	~ColorHolder();

	static void ResetHolder() { holderColor_ = GameColor::kWhite; }

	static GameColor::Color GetHolderColor() { return holderColor_; }

	void Initialize();

	void Update();

	void Draw(Camera* camera);

	void DrawLine(Camera* camera);

private:

	void OnCollision(Collider* collider);

private:

	static GameColor::Color holderColor_;

	std::unique_ptr<Model> modelOuter_;
	std::unique_ptr<Model> modelInner_;

	std::unique_ptr<BoxCollider> collider_;

	std::unique_ptr<LineBox> lineBox_;

	int32_t coolTime_ = 30;

	int32_t countTimer_ = 0;

};


