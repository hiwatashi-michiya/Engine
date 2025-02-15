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
#include "PlayerState.h"
#include <optional>
#include <vector>

/// <summary>
/// プレイヤー全体の処理をまとめたクラス
/// </summary>
class Player : public GameObject
{
public:

	Player();
	~Player();

	//初期化
	void Initialize() override;
	//更新
	void Update() override;
	//描画
	void Draw(Camera* camera) override;
	//当たり判定の位置を調整
	void SetColliderPosition(const Vector3& position) { collider_->collider_.center = position; }
	//オブジェクトの位置を調整し、その後当たり判定をオブジェクトの位置に合わせる
	void SetPosition(const Vector3& position) {
		transform_->translate_ = position;
		transform_->UpdateMatrix();
		collider_->collider_.center = transform_->worldMatrix_.GetTranslate() + Vector3{ 0.0f, collider_->collider_.size.y, 0.0f };
	}
	//コライダー取得
	BoxCollider* GetCollider() const { return collider_.get(); }
	//オブジェクトの位置取得
	Vector3 GetPosition() const { return collider_->collider_.center; }
	//スケール取得
	const Vector3& GetScale() { return transform_->scale_; }
	//落下速度設定。主に落下先にオブジェクトがあった場合のリセットに使用
	void SetVelocityY(float speed) { velocity_.y = speed; }
	//死亡フラグのゲッター、セッター
	bool GetIsDead() const { return isDead_; }
	void SetIsDead(bool flag) { isDead_ = flag; }
	//カメラをプレイヤーに共有。主にカメラ回転時の移動方向調整に使用
	void SetCamera(Camera* camera) { camera_ = camera; }
	//ゴールフラグのゲッター
	bool GetIsGoal() const { return isGoal_; }
	//ゴール可能フラグのゲッター
	bool GetCanGoal() const { return canGoal_; }
	//ゴール可能フラグセッター
	void SetCanGoal(bool flag) { canGoal_ = flag; }
	//地面フラグのゲッター
	bool GetOnGround() const { return onGround_; }
	//ゴールに必要なインクの数を設定
	void SetGoalCount(int32_t count) { goalCount_ = count; }
	//Velocityのゲッター
	Vector3 GetVelocity() const { return velocity_; }
	//Velocityセッター
	void SetVelocity(const Vector3& vel) { velocity_ = vel; }
	//プレイヤーの回転向きを取得
	Quaternion GetRotate() const { return transform_->rotateQuaternion_; }
	//インクの取得数増加
	void AddItemCount() { itemGetCount_++; }
	//プレイヤーの手前当たり判定の座標
	float GetMinZ() const { return collider_->collider_.center.z - collider_->collider_.size.z; }
	//プレイヤーの奥当たり判定の座標
	float GetMaxZ() const { return collider_->collider_.center.z + collider_->collider_.size.z; }
	//プレイヤーの下当たり判定の座標
	float GetMinY() const { return collider_->collider_.center.y - collider_->collider_.size.y; }
	//プレイヤーの上当たり判定の座標
	float GetMaxY() const { return collider_->collider_.center.y + collider_->collider_.size.y; }
	//プレイヤーの位置補正
	void CorrectionPosition();
	//アニメーションセット
	void SetAnimation(int32_t number, bool flag, float speed);
	//プレイヤーの次のステートを設定
	void SetPlayerState(std::unique_ptr<PlayerState> nextState);
	//プレイヤーの落下速度取得
	float GetFallSpeed() const { return fallSpeed_; }
	//プレイヤーの移動速度取得
	float GetSpeed() const { return speed_; }
	//プレイヤーの持っているカメラ取得
	Camera* GetCamera() const { return camera_; }
	//トランスフォーム取得
	Transform* GetTransform() { return transform_.get(); }
	//モデル取得
	SkinningModel* GetModel() { return model_.get(); }
	//ダイブフラグ取得
	bool GetIsDiving() const { return isDiving_; }
	//ダイブフラグセッター
	void SetIsDiving(bool flag) { isDiving_ = flag; }
	//ダイブ可能フラグゲッター、セッター
	void SetCanDive(bool flag) { canDive_ = flag; }
	bool GetCanDive() const { return canDive_; }
	//ダイブカウントゲッター、セッター
	int32_t GetDiveCount() const { return diveCount_; }
	void SetDiveCount(int32_t num) { diveCount_ = num; }
	//タグのセット
	void SetTag(const std::string& tag) { name_ = tag; }

private:
	//他のオブジェクトに当たった時の処理
	void OnCollision(Collider* collider);

	//潜行状態でブロックにぶつかった時の移動方向回転処理
	void RotateVelocity();

private:

	Input* input_ = nullptr;

	Camera* camera_ = nullptr;

	std::unique_ptr<SkinningModel> model_;

	Particle* particle_ = nullptr;

	std::unique_ptr<LineBox> lineBox_;

	std::unique_ptr<PlayerState> state_;

	//ゴールに必要なアイテム数
	int32_t goalCount_ = 0;
	//アイテムの取得数
	int32_t itemGetCount_ = 0;

	Quaternion rotation_;

	std::unique_ptr<BoxCollider> collider_;

	Vector3 preTranslate_{};

	Vector3 velocity_{};
	//移動速度
	float speed_ = 0.35f;
	//落下速度
	float fallSpeed_ = -0.1f;
	//落下速度上限
	float fallSpeedLimit_ = -2.0f;
	//死亡する高さ
	float deadHeight_ = -20.0f;
	//補正時に使う地面の位置
	Vector3* groundPosition_ = nullptr;
	//補正時に使う地面のサイズ
	Vector3* groundSize_ = nullptr;
	//テクスチャ
	Texture* tex_ = nullptr;

	//速度の境界値
	const float kSpeedBorder_ = 1.98f;
	//ブロックの当たり判定の高さ補正値
	const float kHeightCorrectValue_ = 0.5f;
	//ブロックの当たり判定の横補正値
	const float kWidthCorrectValue_ = 0.99f;
	//潜行残り時間カウント
	int32_t diveCount_ = 30;
	//潜行の中断時間
	static const int32_t poseDiveCount_ = 5;

	//ゴール可能かどうか
	bool canGoal_ = false;
	//ゴール判定
	bool isGoal_ = false;
	//地面判定
	bool onGround_ = false;
	//ブロックに潜っている判定
	bool isDiving_ = false;
	//ダイブ可能状態かどうか
	bool canDive_ = false;
	//死亡フラグ
	bool isDead_ = false;

};

