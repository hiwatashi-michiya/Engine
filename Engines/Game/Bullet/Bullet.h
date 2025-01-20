#pragma once
#include "Vector3.h"
#include "Model.h"
#include <memory>
#include "Camera.h"
#include "GameObject.h"
#include "Collider.h"
#include "Game/ColorSetter/ColorSetter.h"

/// <summary>
/// 弾のインターフェース
/// </summary>
class IBullet : public GameObject
{
public:
	~IBullet() {};
	//死亡フラグ取得
	bool GetIsDead() const { return isDead_; }

protected:
	//オブジェクトと当たった時の処理
	virtual void OnCollision(Collider* collider) = 0;

protected:

	//移動向き
	Vector3 velocity_{};
	//移動速度
	float speed_ = 1.0f;
	//生存時間
	float lifeTime_ = 60;
	//モデル
	std::unique_ptr<Model> model_;
	//コライダー
	std::unique_ptr<BoxCollider> collider_;
	//死亡フラグ
	bool isDead_ = false;

};

/// <summary>
/// プレイヤーがブロックの色を変える時に放つ弾のクラス
/// </summary>
class PlayerBullet : public IBullet
{
public:
	PlayerBullet();
	~PlayerBullet();
	
	void Initialize() override;

	void Update() override;

	void Draw(Camera* camera) override;
	//弾の移動方向、初期座標、生存時間セット
	void SetState(const Vector3& velocity, const Vector3& startPosition, float lifeTime);

	void OnCollision(Collider* collider) override;
	//弾の色取得
	GameColor::Color GetBulletColor() const { return bulletColor_; }

private:

	GameColor::Color bulletColor_ = GameColor::kWhite;

};

