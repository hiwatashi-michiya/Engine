#pragma once
#include <wrl.h>
#include <d3d12.h>
#include <string>
#include <vector>
#include <dxcapi.h>
#include "manager/ModelManager.h"
#include "base/Camera.h"
#include "Mesh.h"
#include <memory>
#include <unordered_map>

//ディレクトリパス
const std::string modelDirectoryPath = "resources";

class Model
{
public:

	//モデル全体の初期化
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

	//モデル生成
	static Model* Create(const std::string& filename);

	//描画前処理
	static void PreDraw(ID3D12GraphicsCommandList* commandList);

	//描画後処理
	static void PostDraw();

	static void Finalize();

	//モデル全体に影響を与えるImGui
	static void StaticImGuiUpdate();

	//全体のブレンドモード変更
	static void SetBlendMode(BlendMode blendMode) { currentBlendMode_ = blendMode; }

public:

	void Initialize(const std::string& filename);

	void Draw(Camera* camera);

	void SetTexture(Texture* texture) { texture_ = texture; }

	//メッシュ切り替え
	void SetMesh(const std::string& objFileName);

	//ImGui表示
	void ImGuiUpdate(const std::string& name);

	Vector3 position_{};

	Vector3 rotation_{};

	Vector3 scale_{};

	Matrix4x4 matWorld_{};

	Model* parent_ = nullptr;

	//メッシュ
	Mesh* mesh_;

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

	static std::unordered_map<std::string, std::unique_ptr<Mesh>> meshes_;

private:

	//カメラ座標バッファ
	Microsoft::WRL::ComPtr<ID3D12Resource> cameraBuff_;
	//画面上のワールド座標バッファ
	Microsoft::WRL::ComPtr<ID3D12Resource> matBuff_;
	
	//TransformMatrix
	TransformationMatrix* matTransformMap_ = nullptr;
	//カメラ座標マップ
	CameraForGPU* cameraMap_ = nullptr;

	//テクスチャ
	Texture* texture_;

};

