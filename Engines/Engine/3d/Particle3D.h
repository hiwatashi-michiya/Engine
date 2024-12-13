#pragma once
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
#include "Transform.h"

/// <summary>
/// 3D空間上のパーティクル描画クラス
/// </summary>
class Particle3D
{
public:

	//静的初期化
	static void StaticInitialize(ID3D12Device* device);
	//GPUに使う用のパーティクルデータ
	struct ParticleForGPU {
		Matrix4x4 WVP;
		Matrix4x4 World;
		Matrix4x4 WorldInverseTranspose;
		Vector4 color;
	};
	//ブレンドモード
	enum BlendMode {
		kNormal,//通常
		kAdd,//加算
		kSubtract,//減算
		kMultiply,//乗算
		kScreen,//スクリーン

		kCountBlend
	};
	//ブレンドモードの名前
	static const char* BlendTexts[BlendMode::kCountBlend];
	//生成
	static Particle3D* Create(const std::string& filename, uint32_t instanceCount = 1);
	//描画前処理
	static void PreDraw(ID3D12GraphicsCommandList* commandList);
	//描画後処理
	static void PostDraw();
	//終了処理
	static void Finalize();
	//ImGuiの更新
	static void StaticImGuiUpdate();
	//ブレンドモード切り替え
	static void SetBlendMode(BlendMode blendMode) { currentBlendMode_ = blendMode; }

public:
	//初期化
	void Initialize(const std::string& filename, uint32_t instanceCount);
	//描画コマンド積む
	void Draw(Camera* camera);
	//実際の描画
	void Render();
	//ImGui更新
	void ImGuiUpdate();
	//テクスチャセット
	void SetTexture(Texture* texture) { texture_ = texture; }
	//テクスチャセット
	void SetTexture(const std::string& name);
	//テクスチャパス取得
	const std::string& GetTexturePath() { return texturePath_; }
	//インスタンスカウントセット
	void SetInstanceCount(uint32_t num) { instanceCount_ = num; }
	//パーティクルが1つでもアクティブ状態ならtrue
	bool IsAnyActive();

	//ビルボードを使うかどうか
	bool isBillboard_ = true;
	//トランスフォームリスト
	std::vector<std::shared_ptr<Transform>> transforms_{};
	//色リスト
	std::vector<Vector4> colors_{};
	//ワールド行列リスト
	std::vector<Matrix4x4> worldMatrices{};
	//速度リスト
	std::vector<Vector3> velocities_{};
	//アクティブフラグリスト
	std::vector<bool> isActive_{};
	//生存時間リスト
	std::vector<int32_t> lifeTimes_{};

	//インスタンシングの数
	uint32_t maxInstanceCount_;

	//表示するインスタンスの数
	uint32_t instanceCount_ = 0;

	//メッシュ
	Mesh* mesh_;

	//マテリアル
	std::unique_ptr<Material> material_;

private:

	//静的メンバ変数

	//デバイス
	static ID3D12Device* device_;
	//コマンドリスト
	static ID3D12GraphicsCommandList* commandList_;
	//ルートシグネチャ
	static ID3D12RootSignature* rootSignature_;
	//PSO
	static ID3D12PipelineState* particlePipelineStates_[BlendMode::kCountBlend];

	/*static ID3D12PipelineState* pipelineState_;*/

	static IDxcBlob* vs3dParticleBlob_;
	static IDxcBlob* ps3dParticleBlob_;

	static BlendMode currentBlendMode_;

	static std::unordered_map<std::string, std::shared_ptr<Mesh>> meshes_;

private:


	Microsoft::WRL::ComPtr<ID3D12Resource> matBuff_;
	
	//TransformMatrix
	ParticleForGPU* matTransformMap_ = nullptr;

	//テクスチャ
	Texture* texture_;

	std::string texturePath_;

	//ビルボード行列
	Matrix4x4 matBillboard_;

	//インスタンシングリソース
	InstancingResource instancingResource_;

};

