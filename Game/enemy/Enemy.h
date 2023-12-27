#pragma once
#include "Engine/3d/Model.h"
#include "Engine/2d/Sprite.h"
#include <memory>
#include "Game/block/block.h"
#include "Game/player/Player.h"
#include "Engine/math/Collision.h"

class Enemy
{
public:
	Enemy();
	~Enemy();

	void Initialize();

	void Update();

	void Draw(Camera* camera);

	void SetBlockList(std::list<std::shared_ptr<Block>>* ptr) { blocksPtr_ = ptr; }

	void SetPlayer(Player* player) { player_ = player; }

	bool GetIsDead() const {}

	const Sphere& GetCollision() { return collision_; }

private:

	void Attack();

private:


	struct WorkAttack
	{
		uint32_t attackInterval; //攻撃間隔
		uint32_t attackCount; //攻撃回数(ブロックを出す数)
		int32_t attackTimer; //攻撃するまでのカウント
	};

	std::unique_ptr<Model> model_;

	Player* player_ = nullptr;

	//ブロックリストのポインタ。ブロックを追加するのに使用
	std::list<std::shared_ptr<Block>>* blocksPtr_ = nullptr;

	WorkAttack workAttack_;

	//速度
	Vector3 velocity_{};

	uint32_t kMaxHp_ = 30;

	int32_t hp_ = kMaxHp_;

	bool isDead_ = false;

	//移動先の位置
	Vector3 movePosition_{};

	//当たり判定(球)
	Sphere collision_{};

};
