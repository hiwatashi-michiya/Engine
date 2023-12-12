#pragma once
#include <wrl.h>
#include <d3d12.h>
#include <string>
#include <vector>
#include <dxcapi.h>
#include "ModelManager.h"
#include "WorldTransform.h"
#include "Engine/base/Camera.h"
#include "Mesh.h"
#include <memory>
#include <unordered_map>

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

	/*void SetTexture(Texture* texture) { texture_ = texture; }*/

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

	//モデル識別用変数(ImGuiで使用)
	static int modelNumber_;

	static BlendMode currentBlendMode_;

	static std::unordered_map<std::string, std::shared_ptr<Mesh>> meshes_;

private:

	//カメラ座標バッファ
	Microsoft::WRL::ComPtr<ID3D12Resource> cameraBuff_;
	//画面上のワールド座標バッファ
	Microsoft::WRL::ComPtr<ID3D12Resource> matBuff_;
	
	//TransformMatrix
	TransformationMatrix* matTransformMap_ = nullptr;
	//カメラ座標マップ
	CameraForGPU* cameraMap_ = nullptr;

	//メッシュ
	Mesh* mesh_;

};

