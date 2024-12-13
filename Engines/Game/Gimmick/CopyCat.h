#pragma once
#include "Particle.h"
#include "Model.h"
#include "Input.h"
#include "Collision.h"
#include "Quaternion.h"
#include "Skinning/SkinningModel.h"
#include "Transform.h"
#include <memory>
#include "Particle3D.h"
#include "GameObject.h"
#include "Collider.h"
#include "LineDrawer.h"
#include "Game/player/Player.h"
#include "Game/ColorSetter/ColorSetter.h"

/// <summary>
/// プレイヤーの分身となるオブジェクトのクラス。基本プレイヤーと同じ
/// </summary>
class CopyCat : public GameObject
{
public:
	CopyCat();
	~CopyCat();
	//初期化
	void Initialize();
	//更新
	void Update();
	//描画
	void Draw(Camera* camera);
	//コライダー位置セット
	void SetColliderPosition(const Vector3& position) { collider_->collider_.center = position; }
	//オブジェクトの位置セット
	void SetPosition(const Vector3& position) {
		transform_->translate_ = position;
		transform_->UpdateMatrix();
		collider_->collider_.center = transform_->worldMatrix_.GetTranslate() + Vector3{ 0.0f, collider_->collider_.size.y, 0.0f };
	}
	//分身が死亡した時のリスポーン地点設定
	void SetRespawnPosition(const Vector3& position) { respawnPosition_ = position; }
	//コライダー取得
	BoxCollider* GetCollider() const { return collider_.get(); }
	//オブジェクトの位置取得
	Vector3 GetPosition() const { return collider_->collider_.center; }
	//スケールゲッター
	const Vector3& GetScale() { return transform_->scale_; }
	//落下速度設定
	void SetVelocityY(float speed) { velocity_.y = speed; }
	//死亡フラグ取得
	bool GetIsDead() const { return isDead_; }
	//死亡フラグセッター
	void SetIsDead(bool flag) { isDead_ = flag; }
	//カメラセット
	void SetCamera(Camera* camera) { camera_ = camera; }
	//移動向き取得
	Vector3 GetVelocity() const { return velocity_; }
	//プレイヤーセット
	void SetPlayer(Player* player) { player_ = player; }
	//色セット
	void SetColor(GameColor::Color color) { color_ = color; }

private:
	//オブジェクトと当たった時の処理
	void OnCollision(Collider* collider);

private:

	Input* input_ = nullptr;

	Camera* camera_ = nullptr;

	Player* player_ = nullptr;

	std::unique_ptr<SkinningModel> model_;

	std::unique_ptr<LineBox> lineBox_;

	//ゴール判定
	bool isGoal_ = false;

	//ゴールに必要なアイテム数
	int32_t goalCount_ = 0;
	//ゴール可能かどうか
	bool canGoal_ = false;

	Quaternion rotation_;

	std::unique_ptr<BoxCollider> collider_;
	//一フレーム前の位置
	Vector3 preTranslate_{};
	//速度
	Vector3 velocity_{};
	//リスポーン地点
	Vector3 respawnPosition_{};
	//速度
	float speed_ = 0.25f;
	//死亡フラグ
	bool isDead_ = false;

	//地面判定
	bool onGround_ = false;

	GameColor::Color color_ = GameColor::kWhite;

};

