#pragma once
#include <wrl.h>
#include <d3d12.h>
#include <string>
#include <vector>
#include <dxcapi.h>
#include "Engine/math/Matrix4x4.h"
#include "Engine/math/Vector4.h"
#include "Engine/math/Vector3.h"
#include "Engine/math/Vector2.h"
#include "WorldTransform.h"
#include <vector>
#include "Engine/base/TextureManager.h"

class Particle3D
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

	enum BlendMode {
		kNormal,//通常
		kAdd,//加算
		kSubtract,//減算
		kMultiply,//乗算
		kScreen,//スクリーン

		kCountBlend
	};

	static const char* BlendTexts[BlendMode::kCountBlend];

	static Particle3D* Create(const std::string& filename, uint32_t instanceCount = 1);

	void Initialize(const std::string& filename, uint32_t instanceCount);

	static void PreDraw(ID3D12GraphicsCommandList* commandList);

	static void PostDraw();

	void Draw(std::vector<WorldTransform> worldTransform);

	static void Finalize();

	static WorldTransform worldTransformCamera_;

	static 	Matrix4x4 matProjection_;

	static void StaticImGuiUpdate();

	void ImGuiUpdate();

	static void SetBlendMode(BlendMode blendMode) { currentBlendMode_ = blendMode; }

	//ビルボードを使うかどうか
	bool isBillboard_ = true;

	void SetTexture(Texture& texture) { texture_ = texture; }

	Vector4 color_ = { 1.0f,1.0f,1.0f,1.0f };

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

	//モデル識別用変数(ImGuiで使用)
	static int modelNumber_;

	static BlendMode currentBlendMode_;

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
	Material* constMap_ = nullptr;
	//TransformMatrix
	TransformationMatrix* matTransformMap_ = nullptr;

	//頂点バッファビュー
	D3D12_VERTEX_BUFFER_VIEW vbView_{};
	//インデックスバッファビュー
	D3D12_INDEX_BUFFER_VIEW ibView_{};

	//ビルボード行列
	Matrix4x4 matBillboard_;

	//テクスチャ
	Texture texture_;

	//インスタンシングリソース
	InstancingResource instancingResource_;

	//モデルデータ
	ModelData modelData_;

	//インスタンシングの数
	uint32_t instanceCount_;

};

