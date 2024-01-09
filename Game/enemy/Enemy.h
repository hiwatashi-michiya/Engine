#pragma once
#include "Engine/3d/Model.h"
#include "Engine/2d/Sprite.h"
#include <memory>
#include "Game/block/block.h"
#include "Game/player/Player.h"
#include "Engine/math/Collision.h"
#include "EnemyBullet.h"
#include "Engine/manager/AudioManager.h"

class Enemy
{
public:
	Enemy();
	~Enemy();

	void Initialize();

	void Update();

	void Draw(Camera* camera);

	void DrawTexture();

	void SetBlockList(std::list<std::shared_ptr<Block>>* ptr) { blocksPtr_ = ptr; }

	void SetPlayer(Player* player) { player_ = player; }

	bool GetIsDead() const { return isDead_; }

	const AABB& GetCollision() { return collision_; }

	void Damage(int32_t val) { hp_ -= val; }

	const std::list<std::shared_ptr<EnemyBullet>>& GetBullets() { return bullets_; }

private:

	void Attack();

	void AddBullet();

private:

	AudioManager* audioManager_ = nullptr;

	struct WorkAttack
	{
		uint32_t attackInterval; //攻撃間隔
		uint32_t attackCount; //攻撃回数(ブロックを出す数)
		int32_t attackTimer; //攻撃するまでのカウント
		bool isStartAttack; //攻撃開始フラグ
		uint32_t startAttackInterval; //攻撃中のフレーム数
		int32_t startAttackTimer; //攻撃中のカウント
	};

	struct WorkShot
	{
		uint32_t shotInterval; //発射間隔
		uint32_t shotCount; //弾を発射する数
		int32_t shotTimer; //弾の発射管理をするタイマー
	};

	std::unique_ptr<Model> model_;

	//攻撃場所を表示する用のモデル
	std::array<std::shared_ptr<Model>, 10> attackModels_;

	Player* player_ = nullptr;

	//ブロックリストのポインタ。ブロックを追加するのに使用
	std::list<std::shared_ptr<Block>>* blocksPtr_ = nullptr;

	//敵弾
	std::list<std::shared_ptr<EnemyBullet>> bullets_;

	WorkAttack workAttack_;

	WorkShot workShot_;

	//速度
	Vector3 velocity_{};

	uint32_t kMaxHp_ = 150;

	int32_t hp_ = kMaxHp_;

	//1hp辺りの長さ
	float hpWidth_ = float(300.0f / kMaxHp_);

	bool isDead_ = false;

	//移動先の位置
	Vector3 movePosition_{};

	//当たり判定(球)
	AABB collision_{};

	//攻撃する位置を格納する配列
	std::array<Vector3, 10> attackPositions_;
	//攻撃のサイズ
	std::array<Vector3, 10> attackSizes_;
	//攻撃の当たり判定
	std::array<AABB, 10> attackCollisions_;

	Texture* attackLangeTex_ = nullptr;

	Texture* blockTex_ = nullptr;

	Texture* hpTex_;

	std::unique_ptr<Sprite> hpSprite_;

	uint32_t groundAttackSE_;
	uint32_t shotSE_;
	uint32_t deathSE_;

};
