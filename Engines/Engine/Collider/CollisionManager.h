#pragma once
#include "Collider.h"
#include <list>
#include <mutex>

class CollisionManager {
public:

	static CollisionManager* GetInstance();

	void Initialize() { colliders_.clear(); }

	//コライダー追加
	void PushCollider(Collider* collider);

	//コライダー削除
	void PopCollider(Collider* collider);

	void ClearList();

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

	CollisionManager() = default;
	~CollisionManager() = default;
	CollisionManager(const CollisionManager&) = delete;
	const CollisionManager& operator=(const CollisionManager&) = delete;

};
