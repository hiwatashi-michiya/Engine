#pragma once
#include "Transform.h"
#include <memory>
#include <string>
#include "Camera.h"

/// <summary>
/// 当たり判定の管理をより円滑に行うためのゲームオブジェクトクラス
/// </summary>
class GameObject
{
public:
	GameObject() : transform_(std::make_unique<Transform>()) {};
	virtual ~GameObject() = default;
	
	//初期化
	virtual void Initialize() = 0;
	//更新
	virtual void Update() = 0;
	//描画
	virtual void Draw(Camera* camera) = 0;

	//名前の取得
	const std::string& GetName() { return name_; }
	//トランスフォーム情報の取得
	Transform* GetTransform() { return transform_.get(); }
	//位置セット
	void SetPosition(const Vector3& position) { transform_->translate_ = position; }

protected:
	//トランスフォーム
	std::unique_ptr<Transform> transform_;
	//名前
	std::string name_;

};


