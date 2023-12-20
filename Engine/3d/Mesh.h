#pragma once
#include <wrl.h>
#include <d3d12.h>
#include "Engine/manager/ModelManager.h"
#include "Material.h"
#include <memory>

class Mesh
{
public:

	static void StaticInitialize(ID3D12Device* device);

	Mesh* Create(const std::string& filename);

	//マテリアル
	std::unique_ptr<Material> material_;

	void LoadObjFile(const std::string& filename);

	//描画コマンドセット
	void SetCommandMesh(ID3D12GraphicsCommandList* commandList);

	//描画コマンドセット(パーティクル)
	void SetCommandMesh(ID3D12GraphicsCommandList* commandList, uint32_t instanceCount);

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

	void SetTexture(Texture* texture) { material_->SetTexture(texture); }

private:

	static ID3D12Device* device_;

private:

	//メッシュデータ
	MeshData meshData_;

};

