#pragma once
#include "Animation/Animation.h"
#include "Camera.h"
#include "Drawing/ModelManager.h"
#include "Mesh.h"
#include "SkinCluster.h"
#include <array>
#include <d3d12.h>
#include <dxcapi.h>
#include <memory>
#include <string>
#include <unordered_map>
#include <unordered_map>
#include <vector>
#include <wrl.h>


class SkinningModel
{
public:

	//モデル全体の初期化
	static void StaticInitialize(ID3D12Device* device);

	enum BlendMode {
		kNormal,//通常
		kAdd,//加算
		kSubtract,//減算
		kMultiply,//乗算
		kScreen,//スクリーン

		kCountBlend
	};

	static const char* BlendTexts[BlendMode::kCountBlend];

	//モデル生成
	static SkinningModel* Create(const std::string& filename, int32_t number);

	//描画前処理
	static void PreDraw(ID3D12GraphicsCommandList* commandList);

	//描画後処理
	static void PostDraw();

	static void Finalize();

	//モデル全体に影響を与えるImGui
	static void StaticImGuiUpdate();

	//全体のブレンドモード変更
	static void SetBlendMode(BlendMode blendMode) { currentBlendMode_ = blendMode; }

public:

	void Initialize(const std::string& filename, int32_t number);

	void Draw(Camera* camera);

	void DrawSkeleton(Camera* camera);

	void Draw(const Matrix4x4& localMatrix, Camera* camera);

	void SetTexture(Texture* texture) { texture_ = texture; }

	//メッシュ切り替え
	void SetMesh(const std::string& objFileName);

	//ライト切り替え
	void SetLight(bool flag) { material_->constMap_->enableLighting = flag; }

	//色変更
	void SetColor(const Vector4& color) { material_->constMap_->color = color; }

	//ImGui表示
	void ImGuiUpdate(const std::string& name);

	void SetWorldMatrix(const Matrix4x4& matrix) { worldMatrix_ = matrix; }

	//アニメーションのロード、切り替え時に数字を使う
	void LoadAnimation(const std::string& filename, int32_t number);

	//アニメーションを指定した数字のものに切り替え
	void SetAnimation(int32_t number, bool isReset = true);

	Animation* GetAnimation() { return animations_[currentFileName_].get(); }

	Skeleton* GetSkeleton() { return skeleton_.get(); }

	Matrix4x4 GetSkeletonSpaceMatrix(std::string name) const { return skeleton_->joints[skeleton_->jointMap[name]].skeletonSpaceMatrix; }

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

	Matrix4x4 worldMatrix_;

	SkinningModel* parent_ = nullptr;

	//メッシュ
	Mesh* mesh_;

	//マテリアル
	std::unique_ptr<Material> material_;

	//現在使用しているメッシュのパス
	std::string meshFilePath_;

private:

	//静的メンバ変数

	//デバイス
	static ID3D12Device* device_;
	//コマンドリスト
	static ID3D12GraphicsCommandList* commandList_;
	//ルートシグネチャ
	static ID3D12RootSignature* rootSignature_;
	//PSO
	static ID3D12PipelineState* pipelineStates_[BlendMode::kCountBlend];

	static IDxcBlob* vs3dBlob_;
	static IDxcBlob* ps3dBlob_;

	//モデル識別用変数(ImGuiで使用)
	static int modelNumber_;

	static BlendMode currentBlendMode_;

private:

	//カメラ座標バッファ
	Microsoft::WRL::ComPtr<ID3D12Resource> cameraBuff_;
	//画面上のワールド座標バッファ
	Microsoft::WRL::ComPtr<ID3D12Resource> matBuff_;

	//数字に対応したファイル名を返すマップ
	std::unordered_map<int32_t, std::string> stringMap_;

	//アニメーション
	std::unordered_map<std::string, std::unique_ptr<Animation>> animations_;

	//スケルトン
	std::unique_ptr<Skeleton> skeleton_;

	//スキンクラスター
	std::unique_ptr<SkinCluster> skinCluster_;

	//現在再生しているアニメーションのファイル名
	std::string currentFileName_;

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

	//アニメーションのマトリックス
	Matrix4x4 localMatrix_;

	//TransformMatrix
	TransformationMatrix* matTransformMap_ = nullptr;
	//カメラ座標マップ
	CameraForGPU* cameraMap_ = nullptr;

	//テクスチャ
	Texture* texture_;

};

