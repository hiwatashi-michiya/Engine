#pragma once
#include "Animation/Animation.h"
#include "Model.h"
#include "Skinning/SkinningModel.h"
#include "Transform.h"
#include <memory>
#include "GameObject.h"
#include "Collider.h"
#include "LineDrawer.h"

class Ring : public GameObject
{
public:
	Ring();
	~Ring();

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

	void SetColor(int32_t color) { colorNumber_ = color; }

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

	int32_t colorNumber_ = 0;

	uint32_t getSE_;

};


