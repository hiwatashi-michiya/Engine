#pragma once
#include "Camera.h"
#include "Vector3.h"
#include "Vector4.h"
#include <d3d12.h>
#include <dxcapi.h>
#include <stdint.h>
#include <wrl.h>
#include <memory>
#include <array>

class Line
{
public:
	Line();
	~Line();

	static const int32_t kVertexNum_ = 2;

	static void Initialize(ID3D12Device* device);

	static void PreDraw(ID3D12GraphicsCommandList* commandList);

	static void PostDraw();

	void Draw(Camera* camera, const Matrix4x4& matrix = MakeIdentity4x4());

	Vector3 start_;

	Vector3 end_;

	Vector4 color_;

private:

	//デバイス
	static ID3D12Device* device_;
	//コマンドリスト
	static ID3D12GraphicsCommandList* commandList_;
	//ルートシグネチャ
	static ID3D12RootSignature* rootSignature_;
	//PSO
	static ID3D12PipelineState* pipelineState_;

private:

	struct LineVertexData {
		Vector4 position;
		Vector4 color;
	};

	//頂点リソース
	Microsoft::WRL::ComPtr<ID3D12Resource> vertexBuff_;

	D3D12_VERTEX_BUFFER_VIEW vbView_{};

	//ビュープロジェクション行列リソース
	Microsoft::WRL::ComPtr<ID3D12Resource> wvpBuff_;

	Matrix4x4* wvpMatrix_ = nullptr;

	LineVertexData* vertexMap_ = nullptr;

};

class LineBox
{
public:
	LineBox();
	~LineBox();

	void SetOBB(OBB* obb) { obb_ = obb; }

	void Update();

	void Draw(Camera* camera, const Matrix4x4& matrix = MakeIdentity4x4());

private:

	//描画に使用する線の数
	static const uint32_t kMaxLine_ = 12;

	OBB* obb_ = nullptr;

	std::array<std::unique_ptr<Line>, kMaxLine_> lines_;

};

