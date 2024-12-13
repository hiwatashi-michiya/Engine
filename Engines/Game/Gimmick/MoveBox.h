#pragma once
#include "Engine/3d/Model.h"
#include "Collision.h"
#include "Transform.h"
#include "GameObject.h"
#include "Collider.h"
#include "LineDrawer.h"
#include "Game/ColorSetter/ColorSetter.h"

/// <summary>
/// 移動箱だったもの。現在使用していない
/// </summary>
class MoveBox : public GameObject
{
public:
	MoveBox();
	~MoveBox();
	//初期化
	void Initialize();
	//更新
	void Update();
	//描画コマンド積む
	void Draw(Camera* camera);
	//位置セット
	void SetPosition(const Vector3& position) {
		transform_->translate_ = position;
		collider_->collider_.center = transform_->translate_;
	}
	//スケールセット
	void SetScale(const Vector3& scale) { transform_->scale_ = scale; }
	//位置取得
	const Vector3& GetPosition() { return transform_->translate_; }
	//スケール取得
	const Vector3& GetScale() { return transform_->scale_; }
	//コライダー取得
	BoxCollider* GetCollider() { return collider_.get(); }
	//色セット
	void SetColor(GameColor::Color color) { color_ = color; }

private:

	void OnCollision(Collider* collider);

private:

	std::unique_ptr<Model> model_;

	std::unique_ptr<BoxCollider> collider_;

	std::unique_ptr<LineBox> lineBox_;
	//動いているかどうか
	bool isMove_ = false;
	//一フレーム前の位置
	Vector3 preTranslate_{};
	//色
	GameColor::Color color_ = GameColor::kWhite;

	Texture* circleTex_;
	Texture* crossTex_;

};
