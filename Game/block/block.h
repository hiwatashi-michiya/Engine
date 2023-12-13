#pragma once
#include "Engine/3d/Model.h"
#include "Engine/3d/Particle3D.h"
#include "Game/player/Player.h"

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

	void Draw();

	Vector3 GetPosition() {
		return Vector3{
			model_->matWorld_.m[3][0],
			model_->matWorld_.m[3][1],
			model_->matWorld_.m[3][2]
		};
	}

	bool GetIsDead() { return isDead_; }

private:

	std::unique_ptr<Model> model_;

	std::unique_ptr<Particle3D> particle_;

	bool isBreak_ = false;

	bool isDead_ = false;

	Player* player_ = nullptr;

	//ブロックの大きさ
	Vector3 blockSize_{};

	uint32_t sizeX_;
	uint32_t sizeY_;
	uint32_t sizeZ_;

	uint32_t particleCount_;

};

