#pragma once
#include "Collider.h"
#include <list>

class CollisionManager {
public:
	CollisionManager();
	~CollisionManager();

	void PushCollider(Collider* collider);

	void ClearList();

	void CheckAllCollisions();

private:
	// コライダーリスト
	std::list<Collider*> colliders_;

	/// <summary>
	/// コライダーの2つの衝突判定と応答
	/// </summary>
	/// <param name="colliderA">コライダーA</param>
	/// <param name="colliderB">コライダーB</param>
	void CheckCollisionPair(Collider* colliderA, Collider* colliderB);
};
