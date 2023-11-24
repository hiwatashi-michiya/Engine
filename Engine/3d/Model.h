#pragma once
#include <wrl.h>
#include <d3d12.h>
#include "Engine/math/Matrix4x4.h"
#include "Engine/math/Vector4.h"
#include "Engine/math/Vector3.h"
#include "Engine/math/Vector2.h"
#include <string>
#include <vector>
#include <dxcapi.h>
#include"Engine/base/TextureManager.h"
#include "WorldTransform.h"

//ディレクトリパス
const std::string modelDirectoryPath = "resources";

class Model
{
public:

	static void StaticInitialize(ID3D12Device* device);

	struct VertexData {
		Vector4 position;
		Vector2 texcoord;
		Vector3 normal;
	};

	struct Material
	{
		Vector4 color;
		int32_t enableLighting;
		float padding[3];
		Matrix4x4 uvTransform;
	};

	struct DirectionalLight {
		Vector4 color;
		Vector3 direction;
		float intensity;
	};

	struct MaterialData {
		std::string textureFilePath;
	};

	struct ModelData {
		std::vector<VertexData> vertices;
		MaterialData material;
	};

	struct TransformationMatrix {
		Matrix4x4 WVP;
		Matrix4x4 World;
	};

	static Model* Create(const std::string& filename);

	void Initialize(const std::string& filename);

	static void PreDraw(ID3D12GraphicsCommandList* commandList);

	static void PostDraw();

	void Draw(WorldTransform& worldTransform);

	static void Finalize();

	static WorldTransform worldTransformCamera_;

	static Matrix4x4 matView_;

	static 	Matrix4x4 matProjection_;

	static const Matrix4x4& GetViewProjection() { return matView_ * matProjection_; }

	Vector4 color_ = { 1.0f,1.0f,1.0f,1.0f };

	void ImGuiUpdate(const std::string& name);

	void SetTexture(Texture& texture) { texture_ = texture; }

private:

	//静的メンバ変数

	//デバイス
	static ID3D12Device* device_;
	//コマンドリスト
	static ID3D12GraphicsCommandList* commandList_;
	//ルートシグネチャ
	static ID3D12RootSignature* rootSignature_;
	//PSO
	static ID3D12PipelineState* pipelineState_;

	static IDxcBlob* vs3dBlob_;
	static IDxcBlob* ps3dBlob_;

private:

	//頂点バッファ
	Microsoft::WRL::ComPtr<ID3D12Resource> vertBuff_;
	Microsoft::WRL::ComPtr<ID3D12Resource> matBuff_;
	//インデックスバッファ
	Microsoft::WRL::ComPtr<ID3D12Resource> indexBuff_;
	//定数バッファ
	Microsoft::WRL::ComPtr<ID3D12Resource> constBuff_;
	//平行光源バッファ
	Microsoft::WRL::ComPtr<ID3D12Resource> dLightBuff_;
	//頂点バッファマップ
	VertexData* vertMap_ = nullptr;
	//インデックスバッファマップ
	uint32_t* indexMap_ = nullptr;
	//定数バッファマップ
	Material* constMap_ = nullptr;
	//平行光源バッファマップ
	DirectionalLight* dLightMap_ = nullptr;
	//TransformMatrix
	TransformationMatrix* matTransformMap_ = nullptr;

	//頂点バッファビュー
	D3D12_VERTEX_BUFFER_VIEW vbView_{};
	//インデックスバッファビュー
	D3D12_INDEX_BUFFER_VIEW ibView_{};

	//テクスチャ
	Texture texture_;

	//モデルデータ
	ModelData modelData_;

};

