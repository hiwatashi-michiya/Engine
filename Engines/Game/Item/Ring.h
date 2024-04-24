#pragma once
#include "Model.h"
#include "Transform.h"
#include "Animation/Animation.h"
#include <memory>

class Ring
{
public:
	Ring();
	~Ring();

	void Initialize(const Vector3& position);

	void Update();

	void Draw(Camera* camera);

	bool GetIsVanish() const { return isVanish_; }

	const AABB& GetCollision() { return collision_; }

	//ゲットされた時に呼び出される関数
	void Obtained() { isGet_ = true; }

private:

	AABB collision_{};

	std::unique_ptr<Model> model_;

	std::unique_ptr<Transform> transform_;

	//取得されたフラグ
	bool isGet_ = false;

	//リングが消えるフラグ
	bool isVanish_ = false;

};


