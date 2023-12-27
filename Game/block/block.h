#pragma once
#include "Engine/3d/Model.h"
#include "Engine/3d/Particle3D.h"
#include "Game/player/Player.h"
#include "Game/player/Bullet.h"
#include <memory>
#include "Engine/math/Collision.h"

class Block
{
public:
	Block();
	~Block();

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="pos">配置場所</param>
	/// <param name="player">プレイヤーのポインタ</param>
	/// <param name="blockSize">大きさ(整数で)</param>
	void Initialize(const Vector3& pos, Player* player, const Vector3& blockSize);

	void Update();

	void Draw(Camera* camera);

	Vector3 GetPosition() {
		return Vector3{
			model_->matWorld_.m[3][0],
			model_->matWorld_.m[3][1],
			model_->matWorld_.m[3][2]
		};
	}

	bool GetIsDead() { return isDead_; }

	void SetBullet(std::list<std::shared_ptr<Bullet>>& bullets);

	const AABB& GetCollision() { return collision_; }

private:

	std::unique_ptr<Model> model_;

	bool isBreak_ = false;

	bool isDead_ = false;

	Player* player_ = nullptr;

	//ブロックの大きさ
	Vector3 blockSize_{};

	uint32_t sizeX_;
	uint32_t sizeY_;
	uint32_t sizeZ_;

	uint32_t particleCount_;

	//当たり判定(aabb)
	AABB collision_{};

};

