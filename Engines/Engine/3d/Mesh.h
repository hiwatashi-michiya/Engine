#pragma once
#include "Drawing/RenderingData.h"
#include "Material.h"
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <d3d12.h>
#include <memory>
#include <wrl.h>

/// <summary>
/// 頂点データを保持するクラス
/// </summary>
class Mesh
{
public:
	//静的初期化
	static void StaticInitialize(ID3D12Device* device);
	//生成
	Mesh* Create(const std::string& filename);
	//データ読み込み
	void LoadModelFile(const std::string& filename);

	//テクスチャ読み込み
	void LoadMaterialTemplateFile(const std::string& filename);

	//描画コマンドセット
	void SetCommandMesh(ID3D12GraphicsCommandList* commandList);

	//描画コマンドセット(パーティクル)
	void SetCommandMesh(ID3D12GraphicsCommandList* commandList, uint32_t instanceCount);

	//描画コマンドセット(スキニング)
	void SetCommandMeshForSkinning(ID3D12GraphicsCommandList* commandList);

	//ImGui表示
	void ImGuiUpdate();

	//頂点バッファ
	Microsoft::WRL::ComPtr<ID3D12Resource> vertBuff_;
	//インデックスバッファ
	Microsoft::WRL::ComPtr<ID3D12Resource> indexBuff_;

	//頂点バッファマップ
	VertexData* vertMap_ = nullptr;
	//インデックスバッファマップ
	uint32_t* indexMap_ = nullptr;

	//頂点バッファビュー
	D3D12_VERTEX_BUFFER_VIEW vbView_{};
	//インデックスバッファビュー
	D3D12_INDEX_BUFFER_VIEW ibView_{};

	std::string textureFilePath_;

	//モデルデータ
	ModelData modelData_;

private:

	static ID3D12Device* device_;
	//ノード読み込み
	Node ReadNode(aiNode* node);

private:

	//メッシュデータ
	MeshData meshData_;

};

