#pragma once
#include <wrl.h>
#include <d3d12.h>
#include <string>
#include <vector>
#include <dxcapi.h>
#include "Drawing/ModelManager.h"
#include "base/Camera.h"
#include "Mesh.h"
#include <memory>
#include <unordered_map>
#include "Animation/Animation.h"

//ディレクトリパス
const std::string modelDirectoryPath = "resources";

class Model
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
	static Model* Create(const std::string& filename);

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

	void Initialize(const std::string& filename);

	void Draw(Camera* camera);

	void Draw(const Matrix4x4& localMatrix, Camera* camera);

	void SetTexture(Texture* texture) { texture_ = texture; }

	//メッシュ切り替え
	void SetMesh(const std::string& objFileName);

	//ライト切り替え
	void SetLight(bool flag) { material_->constMap_->enableLighting = flag; }

	//ImGui表示
	void ImGuiUpdate(const std::string& name);

	void SetWorldMatrix(const Matrix4x4& matrix) { worldMatrix_ = matrix; }

	void LoadAnimation(const std::string& filename);

	//アニメーション開始
	void StartAnimation() { isStartAnimation_ = true; }
	//アニメーション一時停止
	void StopAnimation() { isStartAnimation_ = false; }
	//アニメーションのリセット
	void ResetAnimation();
	//アニメーション速度変更
	void SetAnimationSpeed(float speed) { animationSpeed_ = speed; }

	Matrix4x4 worldMatrix_;

	Model* parent_ = nullptr;

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

	/*static ID3D12PipelineState* pipelineState_;*/

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
	
	//アニメーション
	std::unique_ptr<Animation> animation_;

	//アニメーションタイム
	float animationTime_ = 0.0f;

	//アニメーション速度
	float animationSpeed_ = 1.0f;

	//アニメーション管理フラグ
	bool isStartAnimation_ = false;

	//アニメーションのマトリックス
	Matrix4x4 localMatrix_;

	//TransformMatrix
	TransformationMatrix* matTransformMap_ = nullptr;
	//カメラ座標マップ
	CameraForGPU* cameraMap_ = nullptr;

	//テクスチャ
	Texture* texture_;

};

