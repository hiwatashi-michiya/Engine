#pragma once
#include "Animation/Animation.h"
#include "Model.h"
#include "Skinning/SkinningModel.h"
#include "Transform.h"
#include <memory>
#include "GameObject.h"

class Ring : public GameObject
{
public:
	Ring();
	~Ring();

	void Initialize(const Vector3& position);

	void Update();

	void Draw(Camera* camera);

	bool GetIsVanish() const { return isVanish_; }

	bool GetIsObtained() const { return isObtained_; }

	const AABB& GetCollision() { return collision_; }

	//ゲットされた時に呼び出される関数
	void Obtained();

	void PlaySE();

private:

	AABB collision_{};

	std::unique_ptr<Model> model_;

	//取得されたフラグ
	bool isObtained_ = false;

	//リングが消えるフラグ
	bool isVanish_ = false;

	uint32_t getSE_;

};


