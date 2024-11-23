#pragma once
#include "Model.h"
#include "GameObject.h"
#include "Collider.h"
#include <memory>
#include "LineDrawer.h"
#include "Game/player/Player.h"
#include "Particle3D.h"

/// <summary>
/// アイテムを全て集めたときにプレイヤーが触れる必要のあるゴールのクラス
/// </summary>
class Goal : public GameObject
{
public:
	Goal();
	~Goal();
	//初期化
	void Initialize();
	//更新
	void Update();
	//描画
	void Draw(Camera* camera);
	//オブジェクトの位置セット
	void SetPosition(const Vector3& position) {
		transform_->translate_ = position;
		collider_->collider_.center = transform_->translate_;
		collider_->collider_.size = transform_->scale_;
	}
	//スケールセッター
	void SetScale(const Vector3& scale) { transform_->scale_ = scale; }
	//ポジションのゲッター
	const Vector3& GetPosition() { return transform_->translate_; }
	//スケールのゲッター
	const Vector3& GetScale() { return transform_->scale_; }
	//コライダーを取得
	BoxCollider* GetCollider() { return collider_.get(); }
	//プレイヤーのポインタを渡す
	void SetPlayer(Player* player) { player_ = player; }

private:

	Player* player_ = nullptr;

	std::unique_ptr<Model> model_;

	std::unique_ptr<Particle3D> particle_;

	std::unique_ptr<BoxCollider> collider_;

	std::unique_ptr<LineBox> lineBox_;

	bool isActiveEffect_ = false;

	int32_t particleCount_ = 32;

};


