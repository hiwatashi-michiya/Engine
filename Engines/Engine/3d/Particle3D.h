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

class Particle3D
{
public:

	static void StaticInitialize(ID3D12Device* device);

	struct ParticleForGPU {
		Matrix4x4 WVP;
		Matrix4x4 World;
		Matrix4x4 WorldInverseTranspose;
		Vector4 color;
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

	void SetTexture(const std::string& name);

	const std::string& GetTexturePath() { return texturePath_; }

	void SetInstanceCount(uint32_t num) { instanceCount_ = num; }

	bool IsAnyActive();

	//ビルボードを使うかどうか
	bool isBillboard_ = true;

	std::vector<std::shared_ptr<Transform>> transforms_{};

	std::vector<Vector4> colors_{};

	std::vector<Matrix4x4> worldMatrices{};

	std::vector<Vector3> velocities_{};

	std::vector<bool> isActive_{};

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

	//モデル識別用変数(ImGuiで使用)
	static int modelNumber_;

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

