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
#include <array>
#include <wrl.h>
#include "Transform.h"

/// <summary>
/// 2D空間上のパーティクル描画クラス
/// </summary>
class Particle2D
{
public:

	//静的初期化
	static void StaticInitialize(ID3D12Device* device);
	//GPUに使う用のパーティクルデータ
	struct ParticleForGPU {
		Matrix4x4 WVP;
		std::array<Vector2, 4> texcoords;
		Vector4 color;
		Vector2 size;
	};
	/// <summary>
	/// 頂点データ構造体
	/// </summary>
	struct VertexData {
		Vector4 position; //位置
		Vector2 texcoord; //uv座標
	};
	/// <summary>
	/// 2Dマテリアルデータ
	/// </summary>
	struct Material2D {
		Vector4 color;
		Matrix4x4 uvTransform;
	};
	//生成
	static Particle2D* Create(const std::string& filename, uint32_t instanceCount = 1);
	//描画前処理
	static void PreDraw(ID3D12GraphicsCommandList* commandList);
	//描画後処理
	static void PostDraw();
	//終了処理
	static void Finalize();

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
	std::vector<Vector2> positions_{};
	//色リスト
	std::vector<Vector4> colors_{};
	//ワールド行列リスト
	std::vector<Matrix4x4> worldMatrices_{};
	//速度リスト
	std::vector<Vector3> velocities_{};
	//アクティブフラグリスト
	std::vector<bool> isActive_{};
	//生存時間リスト
	std::vector<int32_t> lifeTimes_{};

	//位置
	Vector2 position_{};

	//サイズ
	Vector2 size_{};

	//スプライト上の表示範囲
	Vector2 viewRect_{};

	//アンカーポイント
	Vector2 anchorPoint_{};

	//UVの位置
	Vector2 uvTranslate_{};

	//UV回転
	float uvRotate_ = 0.0f;

	//UVスケール
	Vector2 uvScale_{ 1.0f,1.0f };

	//色
	Vector4 color_ = { 1.0f,1.0f,1.0f,1.0f };

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
	static ID3D12PipelineState* particlePipelineStates_;

	/*static ID3D12PipelineState* pipelineState_;*/

private:

	//頂点バッファ
	Microsoft::WRL::ComPtr<ID3D12Resource> vertBuff_;
	Microsoft::WRL::ComPtr<ID3D12Resource> matBuff_;
	//インデックスバッファ
	Microsoft::WRL::ComPtr<ID3D12Resource> indexBuff_;
	//定数バッファ
	Microsoft::WRL::ComPtr<ID3D12Resource> constBuff_;
	
	//頂点バッファマップ
	VertexData* vertMap_ = nullptr;
	//インデックスバッファマップ
	uint32_t* indexMap_ = nullptr;
	//定数バッファマップ
	Material2D* constMap_ = nullptr;
	//Sprite用のTransformMatrix
	Matrix4x4* matTransformMap_ = nullptr;

	//頂点バッファビュー
	D3D12_VERTEX_BUFFER_VIEW vbView_{};
	//インデックスバッファビュー
	D3D12_INDEX_BUFFER_VIEW ibView_{};

	//TransformMatrix
	ParticleForGPU* matTransformMap_ = nullptr;

	//テクスチャ
	Texture* texture_;

	std::string texturePath_;

	//インスタンシングリソース
	InstancingResource instancingResource_;

};

