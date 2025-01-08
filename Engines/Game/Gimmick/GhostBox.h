#pragma once
#include "Engine/3d/Model.h"
#include "Collision.h"
#include "Transform.h"
#include "GameObject.h"
#include "Collider.h"
#include "LineDrawer.h"
#include "Vector3.h"
#include "Game/ColorSetter/ColorSetter.h"
#include "Game/Variables/CommonVariables.h"
#include <list>

/// <summary>
/// 色に反応して出現したり消えたりするブロックのクラス
/// </summary>
class GhostBox : public GameObject
{
public:

	//箱リスト
	static std::list<GhostBox*> boxList_;

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
		modelTransform_->translate_ = position;
		collider_->collider_.center = transform_->translate_;
		collider_->collider_.size = transform_->scale_ / 2.0f;
	}
	//スケールセッター
	void SetScale(const Vector3& scale) { 
		transform_->scale_ = scale;
		modelTransform_->translate_ = scale;
	}
	//位置取得
	const Vector3& GetPosition() { return transform_->translate_; }
	//スケールゲッター
	const Vector3& GetScale() { return transform_->scale_; }
	//コライダー取得
	BoxCollider* GetCollider() { return collider_.get(); }
	//色セット
	void SetColor(GameColor::Color color) { color_ = color; }
	//色取得
	const GameColor::Color& GetColor() const { return color_; }
	//2つ目の色セット
	void SetSecondColor(GameColor::Color color) { secondColor_ = color; }
	//2つ目の色取得
	const GameColor::Color& GetSecondColor() const { return secondColor_; }
	//色切り替え演出セット
	void StartColorChange(GameColor::Color color);
	//色が白かどうか
	bool IsWhite() const { return color_ == GameColor::Color::kWhite; }

private:
	//オブジェクトと当たった時の処理
	void OnCollision(Collider* collider);

private:

	std::unique_ptr<Model> model_;

	std::unique_ptr<BoxCollider> collider_;

	std::unique_ptr<LineBox> lineBox_;

	std::unique_ptr<Transform> modelTransform_;

	//モデルの追加スケール
	Vector3 addScale_{};
	//一フレーム前の位置
	Vector3 preTranslate_{};
	//色変化時の変化量
	float changeValue_ = 0.02f;
	//最大値
	float maxValue_ = 1.0f;

	//色
	GameColor::Color color_ = GameColor::kWhite;
	//二つ目の色
	GameColor::Color secondColor_ = GameColor::kWhite;
	
};
