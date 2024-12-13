#pragma once
#include "Engine/3d/Model.h"
#include "Collision.h"
#include "Transform.h"
#include "GameObject.h"
#include "Collider.h"
#include "LineDrawer.h"
#include "Game/ColorSetter/ColorSetter.h"

/// <summary>
/// 二点間を移動可能なワープのクラス
/// </summary>
class Warp : public GameObject
{
public:
	Warp();
	~Warp();
	//初期化
	void Initialize();
	//更新
	void Update();
	//描画
	void Draw(Camera* camera);
	//オブジェクトAの位置セット
	void SetPosition(const Vector3& position) {
		transform_->translate_ = position;
		colliderA_->collider_.center = transform_->translate_;
		colliderA_->collider_.size = transform_->scale_ / 2.0f;
	}
	//Aのスケールセッター
	void SetScale(const Vector3& scale) { transform_->scale_ = scale; }
	//オブジェクトBの位置セット
	void SetPositionB(const Vector3& position) {
		transformB_->translate_ = position;
		colliderB_->collider_.center = transformB_->translate_;
		colliderB_->collider_.size = transformB_->scale_ / 2.0f;
	}
	//Bのスケールセッター
	void SetScaleB(const Vector3& scale) { transformB_->scale_ = scale; }
	//色セット
	void SetColor(GameColor::Color color) { color_ = color; }

private:
	//オブジェクトAに当たった場合の処理
	void OnCollisionA(Collider* collider);
	//オブジェクトBに当たった場合の処理
	void OnCollisionB(Collider* collider);

private:

	//二つ分のモデル、当たり判定を使う

	std::unique_ptr<Model> modelA_;
	std::unique_ptr<Model> modelAWire_;

	std::unique_ptr<Model> modelB_;
	std::unique_ptr<Model> modelBWire_;

	std::unique_ptr<BoxCollider> colliderA_;

	std::unique_ptr<BoxCollider> colliderB_;

	std::unique_ptr<Transform> transformB_;

	std::unique_ptr<LineBox> lineBox_;

	std::unique_ptr<LineBox> lineBoxB_;

	//ワープ可能状態かどうか
	bool isActiveWarp_ = true;
	//1フレーム前のワープ状態フラグ
	bool isPreActiveWarp_ = true;
	//クールタイムをカウントする変数
	int32_t countCoolTimer_ = 0;
	//クールタイム
	int32_t coolTime_ = 60;
	//色変化時の変化量
	float changeValue_ = 0.05f;
	//最大値
	float maxValue_ = 1.0f;
	//最大回転角
	const float kMaxRotate_ = 6.28f;
	//色
	GameColor::Color color_ = GameColor::kWhite;

};
