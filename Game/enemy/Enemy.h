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

	bool GetIsDead() const { return isDead_; }

	const AABB& GetCollision() { return collision_; }

	void Damage(int32_t val) { hp_ -= val; }

private:

	void Attack();

private:


	struct WorkAttack
	{
		uint32_t attackInterval; //攻撃間隔
		uint32_t attackCount; //攻撃回数(ブロックを出す数)
		int32_t attackTimer; //攻撃するまでのカウント
		bool isStartAttack; //攻撃開始フラグ
		uint32_t startAttackInterval; //攻撃中のフレーム数
		int32_t startAttackTimer; //攻撃中のカウント
	};

	std::unique_ptr<Model> model_;

	//攻撃場所を表示する用のモデル
	std::array<std::shared_ptr<Model>, 10> attackModels_;

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
	AABB collision_{};

	//攻撃する位置を格納する配列
	std::array<Vector3, 10> attackPositions_;
	//攻撃のサイズ
	std::array<Vector3, 10> attackSizes_;

	Texture* attackLangeTex_ = nullptr;

	Texture* blockTex_ = nullptr;

};
