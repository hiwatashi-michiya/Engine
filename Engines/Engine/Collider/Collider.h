#pragma once
#include <Vector3.h>
#include <stdint.h>
#include "Collision.h"
#include <variant>
#include <functional>
#include "GameObject.h"

class BoxCollider;
class SphereCollider;

class Collider {
public:
	Collider();
	~Collider();

	// 衝突時に呼ばれる関数
	virtual void OnCollision(Collider* collider) { if (function_) { function_(collider); } }
	// コライダーの中心位置取得
	virtual Vector3 GetPosition() = 0;
	// 衝突属性(自分)を取得
	uint32_t GetCollisionAttribute() { return collisionAttribute_; }
	// 衝突属性(自分)を設定
	void SetCollisionAttribute(uint32_t collisionAttribute) { collisionAttribute_ = collisionAttribute; }
	// 衝突マスク(相手)を取得
	uint32_t GetCollisionMask() { return collisionMask_; }
	// 衝突マスク(相手)を設定
	void SetCollisionMask(uint32_t collisionMask) { collisionMask_ = collisionMask; }
	// 判定処理
	virtual bool CollideWith(Collider* other) = 0;
	// 判定処理(OBB)
	virtual bool CollideWithBox(BoxCollider* box) = 0;
	// 判定処理(球)
	virtual bool CollideWithSphere(SphereCollider* sphere) = 0;
	//アクティブ状態にするかどうか
	void SetIsActive(bool flag) { isActive_ = flag; }
	//アクティブ状態かどうか
	bool GetIsActive() const { return isActive_; }
	//関数セット
	void SetFunction(const std::function<void(Collider*)>& func) { function_ = func; }
	//ゲームオブジェクトゲッター
	GameObject* GetGameObject() { return object_; }
	//ゲームオブジェクトセッター
	void SetGameObject(GameObject* object) { object_ = object; }


protected:

	GameObject* object_ = nullptr;

	//衝突時に呼び出す関数
	std::function<void(Collider*)> function_;

	// 衝突属性(自分)
	uint32_t collisionAttribute_ = 0xffffffff;
	// 衝突マスク(相手)
	uint32_t collisionMask_ = 0xffffffff;

	bool isActive_ = true;

};

class BoxCollider : public Collider {
public:

	Vector3 GetPosition() override { return collider_.center; }
	bool CollideWith(Collider* other) override;
	bool CollideWithBox(BoxCollider* box) override;
	bool CollideWithSphere(SphereCollider* sphere) override;

	OBB collider_;

};

class SphereCollider : public Collider {
public:

	Vector3 GetPosition() override { return collider_.center; }
	bool CollideWith(Collider* other) override { return other->CollideWithSphere(this); }
	bool CollideWithBox(BoxCollider* box) override { return IsCollision(this->collider_, box->collider_); }
	bool CollideWithSphere(SphereCollider* sphere) override { return IsCollision(this->collider_, sphere->collider_); }

	Sphere collider_;

};
