#pragma once
#include "Collider.h"
#include <list>
#include <mutex>

/// <summary>
/// コライダーの当たり判定を管理するクラス
/// </summary>
class CollisionManager {
public:
	//インスタンス取得
	static CollisionManager* GetInstance();
	//初期化
	void Initialize() { colliders_.clear(); }

	//コライダー追加
	void PushCollider(Collider* collider);

	//コライダー削除
	void PopCollider(Collider* collider);
	//リストクリア
	void ClearList();
	//全当たり判定チェック
	void CheckAllCollisions();

private:
	
	// コライダーリスト
	std::list<Collider*> colliders_;

	std::mutex collidersMutex_;

	/// <summary>
	/// コライダーの2つの衝突判定と応答
	/// </summary>
	/// <param name="colliderA">コライダーA</param>
	/// <param name="colliderB">コライダーB</param>
	void CheckCollisionPair(Collider* colliderA, Collider* colliderB);

private:
	//シングルトン化
	CollisionManager() = default;
	~CollisionManager() = default;
	CollisionManager(const CollisionManager&) = delete;
	const CollisionManager& operator=(const CollisionManager&) = delete;

};
