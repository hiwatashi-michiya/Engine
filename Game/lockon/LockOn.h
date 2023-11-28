#pragma once
#include "Engine/2d/Sprite.h"
#include "Game/enemy/Enemy.h"
#include "Engine/3d/Model.h"
#include <memory>
#include <list>

class LockOn
{
public:
	LockOn();
	~LockOn();
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 更新
	/// </summary>
	void Update(const std::list<std::shared_ptr<Enemy>>& enemies,const Matrix4x4& matView);

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

	bool IsLockOn() {

		if (target_) {
			return true;
		}

		return false;

	}

	Vector3 GetTargetPosition() const {

		if (target_) {
			return target_->GetModelWorldPos();
		}

		return Vector3(0.0f, 0.0f, 0.0f);

	}

	void Debug();

private:

	Texture tex_;

	std::unique_ptr<Sprite> lockOnMark_;

	//ロックオン対象
	Enemy* target_ = nullptr;

	//ロックオン可能な対象
	std::vector<std::pair<float, Enemy*>> targets_;

	//自動か手動か
	bool isAuto_ = false;

	//検索
	void Search(const std::list<std::shared_ptr<Enemy>>& enemies, const Matrix4x4& matView);

	//範囲外判定
	bool IsOutOfRange();

	void SearchRange();

	//最小距離
	float minDistance_ = 40.0f;

	//最大距離
	float maxDistance_ = 60.0f;

	//角度範囲
	float sinRange_ = 1.0f / 4.0f;

	//ロックオンのクールタイム
	uint32_t lockOnCoolTime_ = 0;

	uint32_t maxLockOnCoolTime_ = 30;

};


