#include "CollisionManager.h"

CollisionManager* CollisionManager::GetInstance() {
	static CollisionManager instance;
	return &instance;
}

void CollisionManager::PushCollider(Collider* collider) { colliders_.push_back(collider); }

void CollisionManager::PopCollider(Collider* collider) {

	auto it = std::find(colliders_.begin(), colliders_.end(), collider);

	if (it != colliders_.end()) {
		colliders_.erase(it);
	}

}

void CollisionManager::ClearList() { colliders_.clear(); }

void CollisionManager::CheckAllCollisions() {

	// リスト内のペアを総当たり
	std::list<Collider*>::iterator itrA = colliders_.begin();
	for (; itrA != colliders_.end(); ++itrA) {
		// イテレータAからコライダーAを取得
		Collider* colliderA = *itrA;

		// イテレータBはイテレータAの次の要素から回す(重複判定回避)
		std::list<Collider*>::iterator itrB = itrA;
		itrB++;

		for (; itrB != colliders_.end(); ++itrB) {
			// イテレータBからコライダーBを取得
			Collider* colliderB = *itrB;

			// ペアの当たり判定
			CheckCollisionPair(colliderA, colliderB);
		}
	}
}

void CollisionManager::CheckCollisionPair(Collider* colliderA, Collider* colliderB) {

	// 衝突フィルタリング
	if ((colliderA->GetCollisionAttribute() & colliderB->GetCollisionMask()) == 0 ||
	    (colliderB->GetCollisionAttribute() & colliderA->GetCollisionMask()) == 0) {
		return;
	}

	// 球と球の交差判定
	if (colliderA->CollideWith(colliderB)) {
		// コライダーAの衝突時コールバックを呼び出す
		colliderA->OnCollision(/*colliderB*/);
		// コライダーBの衝突時コールバックを呼び出す
		colliderB->OnCollision(/*colliderA*/);
	}
}
