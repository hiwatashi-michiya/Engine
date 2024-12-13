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
/// プレイヤーが触れると死亡するオブジェクトのクラス
/// </summary>
class Enemy : public GameObject
{
public:
	Enemy();
	~Enemy();
	//初期化
	void Initialize();
	//更新
	void Update();
	//描画
	void Draw(Camera* camera);
	//色セット
	void SetColor(GameColor::Color color) { color_ = color; }
	//死亡フラグ取得
	bool GetIsDead() const { return isDead_; }

private:
	//オブジェクトと当たった時の処理
	void OnCollision(Collider* collider);

private:

	std::unique_ptr<Model> model_;
	std::unique_ptr<Model> modelWire_;

	std::unique_ptr<LineBox> lineBox_;

	std::unique_ptr<BoxCollider> collider_;
	//最大回転角
	const float kMaxRotate_ = 6.28f;
	//変化量
	float changeValue_ = 0.05f;
	//最大値
	float maxValue_ = 1.0f;

	//死亡フラグ
	bool isDead_ = false;
	//色
	GameColor::Color color_ = GameColor::kWhite;

};


