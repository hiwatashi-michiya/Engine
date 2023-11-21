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

private:

	Texture tex_;

	std::unique_ptr<Sprite> lockOnMark_;

	//ロックオン対象
	Enemy* target_ = nullptr;

	//検索
	void Search(const std::list<std::shared_ptr<Enemy>>& enemies, const Matrix4x4& matView);

	//範囲外判定
	bool IsOutOfRange();

	void SearchRange();

	//最小距離
	float minDistance_ = 10.0f;

	//最大距離
	float maxDistance_ = 100.0f;

	//角度範囲
	float angleRange_ = 3.14f / 9.0f;

};


