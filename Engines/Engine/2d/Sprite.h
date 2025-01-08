#pragma once
#include "Drawing/TextureManager.h"
#include "Matrix4x4.h"
#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"
#include <d3d12.h>
#include <dxcapi.h>
#include <wrl.h>

/// <summary>
/// 画像の描画を行うクラス
/// </summary>
class Sprite
{
public:

	Sprite(Texture* texture, Vector2 position, Vector2 size, Vector4 color);
	~Sprite() = default;
	//静的初期化
	static void StaticInitialize(ID3D12Device* device);

	/// <summary>
	/// 頂点データ構造体
	/// </summary>
	struct VertexData {
		Vector4 position; //位置
		Vector2 texcoord; //uv座標
	};
	/// <summary>
	/// 2Dマテリアルデータ
	/// </summary>
	struct Material2D {
		Vector4 color;
		Matrix4x4 uvTransform;
	};
	//生成
	static Sprite* Create(Texture* texture, Vector2 position, Vector4 color = { 1.0f,1.0f,1.0f,1.0f });
	//描画前処理
	static void PreDraw(ID3D12GraphicsCommandList* commandList);
	//描画後処理
	static void PostDraw();
	//終了処理
	static void Finalize();

	//位置
	Vector2 position_{};

	//サイズ
	Vector2 size_{};

	//スプライト上の表示範囲
	Vector2 viewRect_{};

	//アンカーポイント
	Vector2 anchorPoint_{};

	//UVの位置
	Vector2 uvTranslate_{};

	//UV回転
	float uvRotate_;

	//UVスケール
	Vector2 uvScale_{ 1.0f,1.0f };

	//色
	Vector4 color_ = { 1.0f,1.0f,1.0f,1.0f };

public:
	//描画コマンド積む
	void Draw();
	//実際の描画
	void Render();
	//テクスチャセット
	void SetTexture(Texture* tex) { texture_ = tex; }
	//ポジションセット
	void SetPosition(Vector2 pos) { position_ = pos; }
	//ポジション取得
	Vector2 GetPosition() { return position_; }
	//デバッグ更新
	void ImGuiUpdate(const std::string name);

private:
	//初期化
	bool Initialize();

private:

	//静的メンバ変数

	//デバイス
	static ID3D12Device* device_;
	//コマンドリスト
	static ID3D12GraphicsCommandList* commandList_;
	//ルートシグネチャ
	static ID3D12RootSignature* rootSignature2D_;
	//PSO
	static ID3D12PipelineState* pipelineState2D_;

	static Matrix4x4 matProjection_;

	static IDxcBlob* vs2dBlob_;
	static IDxcBlob* ps2dBlob_;

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
	Material2D* constMap_ = nullptr;
	//Sprite用のTransformMatrix
	Matrix4x4* matTransformMap_ = nullptr;

	//頂点バッファビュー
	D3D12_VERTEX_BUFFER_VIEW vbView_{};
	//インデックスバッファビュー
	D3D12_INDEX_BUFFER_VIEW ibView_{};


	//テクスチャ
	Texture* texture_;

};

