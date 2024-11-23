#pragma once
#include "Model.h"
#include "GameObject.h"
#include "Collider.h"
#include "LineDrawer.h"
#include "Game/ColorSetter/ColorSetter.h"

/// <summary>
/// 二色目を保有できるオブジェクトのクラス。色はクラス間共有
/// </summary>
class ColorHolder : public GameObject
{
public:
	ColorHolder();
	~ColorHolder();
	//ホルダーの色リセット
	static void ResetHolder() { holderColor_ = GameColor::kWhite; }
	//色取得
	static GameColor::Color GetHolderColor() { return holderColor_; }
	//初期化
	void Initialize();
	//更新
	void Update();
	//描画
	void Draw(Camera* camera);

private:
	//オブジェクトと当たった時の処理
	void OnCollision(Collider* collider);

private:

	static GameColor::Color holderColor_;

	std::unique_ptr<Model> modelOuter_;
	std::unique_ptr<Model> modelInner_;

	std::unique_ptr<BoxCollider> collider_;

	std::unique_ptr<LineBox> lineBox_;
	//クールタイム
	int32_t coolTime_ = 30;
	//クールタイムカウント
	int32_t countTimer_ = 0;

};


