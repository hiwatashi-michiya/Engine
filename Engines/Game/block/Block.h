#pragma once
#include "Engine/3d/Model.h"
#include "Collision.h"
#include "Transform.h"
#include "GameObject.h"
#include "Collider.h"
#include "LineDrawer.h"

/// <summary>
/// プレイヤーの足場や壁となるブロックのクラス
/// </summary>
class Block : public GameObject
{
public:
	Block();
	~Block();
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
		collider_->collider_.size = transform_->scale_ / 2.0f;
	}
	//スケールセッター
	void SetScale(const Vector3& scale) { transform_->scale_ = scale; }
	//位置取得
	const Vector3& GetPosition() { return transform_->translate_; }
	//スケールゲッター
	const Vector3& GetScale() { return transform_->scale_; }
	//コライダー取得
	BoxCollider* GetCollider() { return collider_.get(); }

private:

	std::unique_ptr<Model> model_;

	std::unique_ptr<BoxCollider> collider_;

	std::unique_ptr<LineBox> lineBox_;
	//光の強度
	float pLightIntensity_ = 0.5f;
	//ブロックが光るときのカウント
	int32_t lightCount_ = 0;

	int32_t setCount_;

};
