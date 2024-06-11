#pragma once
#include "Camera.h"
#include "Drawing/ModelManager.h"
#include "Mesh.h"
#include <d3d12.h>
#include <dxcapi.h>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>
#include <vector>
#include <wrl.h>

class Particle3D
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

	static Particle3D* Create(const std::string& filename, uint32_t instanceCount = 1);

	static void PreDraw(ID3D12GraphicsCommandList* commandList);

	static void PostDraw();

	static void Finalize();

	static void StaticImGuiUpdate();

	static void SetBlendMode(BlendMode blendMode) { currentBlendMode_ = blendMode; }

public:

	void Initialize(const std::string& filename, uint32_t instanceCount);

	void Draw(Camera* camera);

	void ImGuiUpdate();

	void SetTexture(Texture* texture) { texture_ = texture; }

	//ビルボードを使うかどうか
	bool isBillboard_ = true;

	std::vector<Vector3> positions_{};

	std::vector<Vector3> rotations_{};

	std::vector<Vector3> scales_{};

	std::vector<Matrix4x4> worldMatrices{};

	std::vector<Vector3> velocities_{};

	//インスタンシングの数
	uint32_t instanceCount_;

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

	//モデル識別用変数(ImGuiで使用)
	static int modelNumber_;

	static BlendMode currentBlendMode_;

	static std::unordered_map<std::string, std::shared_ptr<Mesh>> meshes_;

private:


	Microsoft::WRL::ComPtr<ID3D12Resource> matBuff_;
	
	//TransformMatrix
	TransformationMatrix* matTransformMap_ = nullptr;

	//テクスチャ
	Texture* texture_;

	//ビルボード行列
	Matrix4x4 matBillboard_;

	//インスタンシングリソース
	InstancingResource instancingResource_;

};

