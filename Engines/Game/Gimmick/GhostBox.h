#pragma once
#include "Engine/3d/Model.h"
#include "Collision.h"
#include "Transform.h"
#include "GameObject.h"
#include "Collider.h"
#include "LineDrawer.h"
#include "Vector3.h"
#include "Game/ColorSetter/ColorSetter.h"

/// <summary>
/// 色に反応して出現したり消えたりするブロックのクラス
/// </summary>
class GhostBox : public GameObject
{
public:
	GhostBox();
	~GhostBox();
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
	//色セット
	void SetColor(GameColor::Color color) { color_ = color; }

private:
	//オブジェクトと当たった時の処理
	void OnCollision(Collider* collider);

private:

	std::unique_ptr<Model> model_;
	std::unique_ptr<Model> modelWire_;

	std::unique_ptr<BoxCollider> collider_;

	std::unique_ptr<LineBox> lineBox_;

	Vector3 preTranslate_{};

	GameColor::Color color_ = GameColor::kWhite;

};
