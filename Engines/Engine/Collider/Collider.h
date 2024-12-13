#pragma once
#include <Vector3.h>
#include <stdint.h>
#include "Collision.h"
#include <variant>
#include <functional>
#include "GameObject.h"

class BoxCollider;
class SphereCollider;

/// <summary>
/// コライダーの基底クラス
/// </summary>
class Collider {
public:
	Collider();
	~Collider();

	// 衝突時に呼ばれる関数
	virtual void OnCollision(Collider* collider) { if (function_) { function_(collider); } }
	// コライダーの中心位置取得
	virtual Vector3 GetPosition() = 0;
	//中心位置セット
	virtual void SetPosition(const Vector3& position) = 0;
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

	//サイズ取得(OBB用)
	virtual Vector3 GetSize() const { return Vector3(0.0f, 0.0f, 0.0f); }
	//半径取得(Sphere用)
	virtual float GetRadius() const { return 0.0f; }

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

/// <summary>
/// OBBの当たり判定を持ったコライダー
/// </summary>
class BoxCollider : public Collider {
public:

	Vector3 GetPosition() override { return collider_.center; }
	void SetPosition(const Vector3& position) override { collider_.center = position; }
	bool CollideWith(Collider* other) override;
	bool CollideWithBox(BoxCollider* box) override;
	bool CollideWithSphere(SphereCollider* sphere) override;
	Vector3 GetSize() const override { return collider_.size; }

	OBB collider_;

};

/// <summary>
/// 球体の当たり判定を持ったコライダー
/// </summary>
class SphereCollider : public Collider {
public:

	Vector3 GetPosition() override { return collider_.center; }
	void SetPosition(const Vector3& position) override { collider_.center = position; }
	bool CollideWith(Collider* other) override { return other->CollideWithSphere(this); }
	bool CollideWithBox(BoxCollider* box) override { return IsCollision(this->collider_, box->collider_); }
	bool CollideWithSphere(SphereCollider* sphere) override { return IsCollision(this->collider_, sphere->collider_); }
	float GetRadius() const override { return collider_.radius; }

	Sphere collider_;

};
