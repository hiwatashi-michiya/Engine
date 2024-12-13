#pragma once
#include "GameObject.h"
#include "Collider.h"
#include "LineDrawer.h"

class Player;

/// <summary>
/// プレイヤーが潜るフラグを管理するオブジェクト
/// </summary>
class DiveFlagObject : public GameObject
{
public:

	DiveFlagObject();
	~DiveFlagObject();
	//初期化
	void Initialize(Transform* parent, Player* player);
	//更新
	void Update();
	//描画
	void Draw(Camera* camera);

private:

	//他のオブジェクトに当たった時の処理
	void OnCollision(Collider* collider);

private:

	std::unique_ptr<BoxCollider> collider_;

	std::unique_ptr<LineBox> lineBox_;
	//プレイヤーのポインタ
	Player* player_ = nullptr;

};


