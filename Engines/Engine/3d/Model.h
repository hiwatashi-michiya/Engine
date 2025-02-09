#pragma once
#include "Animation/Animation.h"
#include "Camera.h"
#include "Drawing/RenderingData.h"
#include "Mesh.h"
#include <d3d12.h>
#include <dxcapi.h>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>
#include <wrl.h>
#include "Vector4.h"

class InstancingModel;

/// <summary>
/// モデルの描画を行うクラス
/// </summary>
class Model
{
public:
	//初期化
	void Initialize(const std::string& filename);
	//描画コマンド積む
	void Draw(Camera* camera);
	//メッシュ切り替え
	void SetMesh(const std::string& objFileName);

	//ワールド行列セット
	void SetWorldMatrix(const Matrix4x4& matrix) { worldMatrix_ = matrix; }
	//アニメーション読み込み
	void LoadAnimation(const std::string& filename);

	/// <summary>
	/// アニメーション開始
	/// </summary>
	/// <param name="isLoop">ループするかどうか</param>
	void StartAnimation(bool isLoop = false) { 
		isStartAnimation_ = true;
		isLoop_ = isLoop;
	}
	//アニメーション一時停止
	void StopAnimation() { isStartAnimation_ = false; }
	//アニメーションのリセット
	void ResetAnimation();
	//アニメーション速度変更
	void SetAnimationSpeed(float speed) { animationSpeed_ = speed; }
	//アニメーション更新処理
	void UpdateAnimation();
	//アニメーションの終わりかどうか
	bool IsEndAnimation() const { return isEndAnimation_; }
	//アニメーションのループフラグを設定
	void SetIsLoop(bool flag) { isLoop_ = flag; }
	//色変更
	void SetColor(const Vector4& color) { color_ = color; }
	//インスタンシングモデル取得
	InstancingModel* GetInstancingModel() { return instancingModel_; }
	//ワールド行列
	Matrix4x4 worldMatrix_;
	//アニメーションのマトリックス
	Matrix4x4 localMatrix_;
	//ワールドビュープロジェクション行列
	Matrix4x4 worldViewProjectionMatrix_;
	//色
	Vector4 color_;

private:

	InstancingModel* instancingModel_ = nullptr;

	//アニメーション
	std::unique_ptr<Animation> animation_;

	//スケルトン
	std::unique_ptr<Skeleton> skeleton_;

	//アニメーションタイム
	float animationTime_ = 0.0f;

	//アニメーション速度
	float animationSpeed_ = 1.0f;

	//アニメーション管理フラグ
	bool isStartAnimation_ = false;

	//ループ管理フラグ
	bool isLoop_ = false;

	//アニメーションの終点に到達したかどうか
	bool isEndAnimation_ = false;

};

