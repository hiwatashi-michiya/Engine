#pragma once
#include "Model.h"
#include "GameObject.h"
#include "Collider.h"
#include "LineDrawer.h"
#include "Game/ColorSetter/ColorSetter.h"

/// <summary>
/// ステージの色の切り替えを行うスイッチの役割を持ったクラス
/// </summary>
class Switch : public GameObject
{
public:
	Switch();
	~Switch();
	//初期化
	void Initialize() override;
	//更新
	void Update() override;
	//描画
	void Draw(Camera* camera) override;
	//色セット
	void SetColor(GameColor::Color color) { color_ = color; }
	//コライダー取得
	BoxCollider* GetCollider() { return collider_.get(); }

private:
	//他オブジェクトと当たった時の処理
	void OnCollision(Collider* collider);

private:

	std::unique_ptr<Model> model_;
	std::unique_ptr<Model> modelWire_;

	std::unique_ptr<BoxCollider> collider_;

	std::unique_ptr<LineBox> lineBox_;

	std::unique_ptr<Transform> modelTransform_;

	//モデルの追加スケール
	Vector3 addScale_{};

	//色
	GameColor::Color color_ = GameColor::kWhite;
	//切り替えクールタイム
	int32_t coolTime_ = 30;
	//クールタイムのカウント
	int32_t countTimer_ = 0;
	//プレイヤーが触れたかどうか
	bool isEntered_ = false;
	bool preIsEntered_ = false;

};


