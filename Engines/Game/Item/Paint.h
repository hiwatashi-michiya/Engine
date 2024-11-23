#pragma once
#include "Animation/Animation.h"
#include "Model.h"
#include "Skinning/SkinningModel.h"
#include "Transform.h"
#include <memory>
#include "GameObject.h"
#include "Collider.h"
#include "LineDrawer.h"
#include "Game/ColorSetter/ColorSetter.h"

/// <summary>
/// プレイヤーがゴールするために集めるアイテムのクラス
/// </summary>
class Paint : public GameObject
{
public:
	Paint();
	~Paint();
	//このオブジェクト共通の名前
	static const std::string objectName_;
	//初期化
	void Initialize(const Vector3& position);
	//更新
	void Update();
	//描画
	void Draw(Camera* camera);
	//消滅フラグのゲッター
	bool GetIsVanish() const { return isVanish_; }
	//取得されたかどうかのフラグのゲッター
	bool GetIsObtained() const { return isObtained_; }
	//コライダー取得
	BoxCollider* GetCollider() const { return collider_.get(); }

	//ゲットされた時に呼び出される関数
	void Obtained();
	//SE再生
	void PlaySE();
	//オブジェクトの色設定
	void SetColor(GameColor::Color color) { color_ = color; }

private:
	//他のオブジェクトに当たった時の処理
	void OnCollision(Collider* collider);

private:

	std::unique_ptr<BoxCollider> collider_;

	std::unique_ptr<LineBox> lineBox_;

	std::unique_ptr<Model> model_;
	std::unique_ptr<Model> modelWire_;

	//取得されたフラグ
	bool isObtained_ = false;

	//リングが消えるフラグ
	bool isVanish_ = false;

	GameColor::Color color_ = GameColor::kWhite;

	uint32_t getSE_;

};


