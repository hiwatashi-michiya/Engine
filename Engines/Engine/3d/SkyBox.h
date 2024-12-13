#pragma once
#include <d3d12.h>
#include <dxcapi.h>
#include <memory>
#include <string>
#include <wrl.h>
#include "Vector4.h"
#include "Camera.h"
#include "TextureManager.h"
#include "Matrix4x4.h"

/// <summary>
/// ddsファイルから画像を読み込んで背景にできるクラス
/// </summary>
class Skybox
{
public:
	Skybox();
	~Skybox();
	//静的初期化
	static void Initialize();
	//描画前処理
	static void PreDraw(ID3D12GraphicsCommandList* commandList);
	//描画後処理
	static void PostDraw();
	//生成
	void Create();
	//dds読み込み
	void LoadDds(const std::string& ddsFileName);
	//描画コマンド積む
	void Draw(Camera* camera);
	//ワールド行列セット
	void SetWorldMatrix(const Matrix4x4& worldMatrix) { worldMatrix_ = worldMatrix; }

private:

	//デバイス
	static ID3D12Device* device_;
	//コマンドリスト
	static ID3D12GraphicsCommandList* commandList_;
	//ルートシグネチャ
	static ID3D12RootSignature* rootSignature_;
	//PSO
	static ID3D12PipelineState* pipelineState_;

private:
	//スカイボックスの頂点データ
	struct SkyboxVertexData{
		Vector4 position;
	};
	//スカイボックスのワールド行列データ
	struct SkyboxTransformationMatrix {
		//ワールドビュープロジェクション
		Matrix4x4 WVP;
	};
	//スカイボックスのマテリアルデータ
	struct SkyboxMaterial {
		Vector4 color;
	};

	Microsoft::WRL::ComPtr<ID3D12Resource> vertexBuff_;
	//インデックスバッファ
	Microsoft::WRL::ComPtr<ID3D12Resource> indexBuff_;
	//トランスフォームバッファ
	Microsoft::WRL::ComPtr<ID3D12Resource> transformationBuff_;
	//マテリアルバッファ
	Microsoft::WRL::ComPtr<ID3D12Resource> materialBuff_;

	SkyboxVertexData* vertexMap_ = nullptr;
	//インデックスバッファマップ
	uint32_t* indexMap_ = nullptr;

	//頂点バッファビュー
	D3D12_VERTEX_BUFFER_VIEW vbView_{};
	//インデックスバッファビュー
	D3D12_INDEX_BUFFER_VIEW ibView_{};

	SkyboxTransformationMatrix* transformationMap_ = nullptr;

	SkyboxMaterial* materialMap_ = nullptr;

	Matrix4x4 worldMatrix_;

	Texture* tex_ = nullptr;

};
