#pragma once
#include "Animation/Animation.h"
#include "Model.h"
#include "Skinning/SkinningModel.h"
#include "Transform.h"
#include <memory>
#include "GameObject.h"
#include "Collider.h"
#include "LineDrawer.h"
#include "Game/ColorSetter/ColorSetter.h"

class Paint : public GameObject
{
public:
	Paint();
	~Paint();

	static const std::string objectName_;

	void Initialize(const Vector3& position);

	void Update();

	void Draw(Camera* camera);

	void DrawLine(Camera* camera);

	bool GetIsVanish() const { return isVanish_; }

	bool GetIsObtained() const { return isObtained_; }

	BoxCollider* GetCollider() const { return collider_.get(); }

	//ゲットされた時に呼び出される関数
	void Obtained();

	void PlaySE();

	void SetColor(GameColor::Color color) { color_ = color; }

private:

	void OnCollision(Collider* collider);

private:

	std::unique_ptr<BoxCollider> collider_;

	std::unique_ptr<LineBox> lineBox_;

	std::unique_ptr<Model> model_;
	std::unique_ptr<Model> modelWire_;

	//取得されたフラグ
	bool isObtained_ = false;

	//リングが消えるフラグ
	bool isVanish_ = false;

	GameColor::Color color_ = GameColor::kWhite;

	uint32_t getSE_;

};


