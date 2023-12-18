#pragma once
#include <wrl.h>
#include <d3d12.h>
#include "ModelManager.h"


class Material
{
public:

	static void StaticInitialize(ID3D12Device* device);

	Material* Create(const std::string& filename);
	
	//マテリアルファイル読み込み
	void LoadMaterialTemplateFile(const std::string& filename);

	//描画コマンドセット
	void SetCommandMaterial(ID3D12GraphicsCommandList* commandList);

	//パーティクル用
	void SetCommandMaterialForParticle(ID3D12GraphicsCommandList* commandList);

	//テクスチャセット
	void SetTexture(Texture* texture) { texture_ = texture; }

	//ImGui表示
	void ImGuiUpdate();

	//定数バッファ
	Microsoft::WRL::ComPtr<ID3D12Resource> constBuff_;
	//平行光源バッファ
	Microsoft::WRL::ComPtr<ID3D12Resource> dLightBuff_;
	//点光源バッファ
	Microsoft::WRL::ComPtr<ID3D12Resource> pLightBuff_;

	//定数バッファマップ
	MaterialData* constMap_ = nullptr;
	//平行光源バッファマップ
	DirectionalLight* dLightMap_ = nullptr;
	//ポイントライト
	PointLight* pLightMap_ = nullptr;

	//テクスチャ
	Texture* texture_;

private:

	static ID3D12Device* device_;

};

