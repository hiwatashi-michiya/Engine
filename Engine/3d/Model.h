#pragma once
#include <wrl.h>
#include <d3d12.h>
#include <string>
#include <vector>
#include <dxcapi.h>
#include "ModelManager.h"
#include "WorldTransform.h"

//ディレクトリパス
const std::string modelDirectoryPath = "resources";

class Model
{
public:

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

	static Model* Create(const std::string& filename);

	void Initialize(const std::string& filename);

	static void PreDraw(ID3D12GraphicsCommandList* commandList);

	static void PostDraw();

	void Draw(WorldTransform worldTransform);

	static void Finalize();

	static WorldTransform worldTransformCamera_;

	static 	Matrix4x4 matProjection_;

	static void StaticImGuiUpdate();

	void ImGuiUpdate();

	static void SetBlendMode(BlendMode blendMode) { currentBlendMode_ = blendMode; }

private:

	//静的メンバ変数

	//デバイス
	static ID3D12Device* device_;
	//コマンドリスト
	static ID3D12GraphicsCommandList* commandList_;
	//ルートシグネチャ
	static Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature_;
	//PSO
	static Microsoft::WRL::ComPtr<ID3D12PipelineState> pipelineStates_[BlendMode::kCountBlend];

	/*static ID3D12PipelineState* pipelineState_;*/

	static Microsoft::WRL::ComPtr<IDxcBlob> vs3dBlob_;
	static Microsoft::WRL::ComPtr<IDxcBlob> ps3dBlob_;

	//共通の平行光源バッファ
	static Microsoft::WRL::ComPtr<ID3D12Resource> dLightBuff_;
	//共通の平行光源バッファマップ
	static DirectionalLight* dLightMap_;

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

	//テクスチャ
	Texture* texture_;

	//モデルデータ
	ModelData modelData_;

};

