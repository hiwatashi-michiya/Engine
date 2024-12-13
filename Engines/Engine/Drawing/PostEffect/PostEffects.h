#pragma once
#include "RenderTexture.h"
#include <string>
#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"
#include "Matrix4x4.h"
#include "Camera.h"

/// <summary>
/// ポストエフェクトの種類
/// </summary>
enum PostEffectType {

	kNone, //エフェクト無し
	kGrayscale, //グレースケール
	kVignette, //ビネット
	kBoxFilter, //ぼかし
	kGaussianFilter, //ガウスぼかし
	kLuminanceBasedOutline, //輝度ベースアウトライン
	kDepthBasedOutline, //深度ベースアウトライン
	kRadialBlur, //放射状ブラー
	kHSVFilter, //HSV
	kDissolve, //ディゾルブ

	kMaxEffects, //エフェクト最大数

};

/// <summary>
/// ポストエフェクトの基底クラス
/// </summary>
class PostEffects
{
public:

	PostEffects() = default;
	~PostEffects() = default;
	//初期化
	virtual void Initialize();
	//生成
	virtual void Create();
	//描画
	virtual void Render();

	//描画後処理
	virtual void PostRender();
	//デバッグ
	virtual void Debug();
	//カメラセット
	void SetCamera(Camera* camera) { camera_ = camera; }
	//ポストエフェクト名
	std::string name_;

	Camera* camera_ = nullptr;

protected:
	//ルートシグネチャ
	ID3D12RootSignature* rootSignature_;
	//パイプライン
	ID3D12PipelineState* pipelineState_;
	//初期化したかどうか
	bool isInitialized_ = false;

};

class CopyImageRender : public PostEffects
{
public:

	CopyImageRender() = default;
	~CopyImageRender() = default;

private:

};


class Grayscale : public PostEffects
{

public:
	//固有のパラメータ
	struct Parameter {
		
	};

public:

	Grayscale() = default;
	~Grayscale() = default;

	void Create() override;

	void Render() override;

	void Debug() override;

	Parameter* parameter_ = nullptr;

private:

	Microsoft::WRL::ComPtr<ID3D12Resource> buffer_;

};

class Vignette : public PostEffects
{

public:
	//固有のパラメータ
	struct Parameter {
		//色
		Vector3 color = { 1.0f,1.0f,1.0f };
		//色の強さ
		float colorPower = 0.2f;
		//スケール
		float scale = 16.0f;
		//ビネット強さ
		float power = 0.8f;
		float padding[2];
	};

public:

	Vignette() = default;
	~Vignette() = default;

	void Create() override;

	void Render() override;

	void Debug() override;

	Parameter* parameter_ = nullptr;

private:

	Microsoft::WRL::ComPtr<ID3D12Resource> buffer_;

};

class BoxFilter : public PostEffects
{

public:
	//固有のパラメータ
	struct Parameter {
		//分割数
		int32_t size = 2;
		float padding[3];
	};

public:

	BoxFilter() = default;
	~BoxFilter() = default;

	void Create() override;

	void Render() override;

	void Debug() override;

	Parameter* parameter_ = nullptr;

private:

	Microsoft::WRL::ComPtr<ID3D12Resource> buffer_;

};

class GaussianFilter : public PostEffects
{

public:
	//固有のパラメータ
	struct Parameter {
		//分割数
		int32_t size = 2;
		//シグマ
		float sigma = 2.0f;
		float padding[2];
	};

public:

	GaussianFilter() = default;
	~GaussianFilter() = default;

	void Create() override;

	void Render() override;

	void Debug() override;

	Parameter* parameter_ = nullptr;

private:

	Microsoft::WRL::ComPtr<ID3D12Resource> buffer_;

};

class LuminanceBasedOutline : public PostEffects
{

public:
	//固有のパラメータ
	struct Parameter {
		//差の倍率
		float multiplier = 6.0f;
		float padding[3];
	};

public:

	LuminanceBasedOutline() = default;
	~LuminanceBasedOutline() = default;

	void Create() override;

	void Render() override;

	void Debug() override;

	Parameter* parameter_ = nullptr;

private:

	Microsoft::WRL::ComPtr<ID3D12Resource> buffer_;
	
};

class DepthBasedOutline : public PostEffects
{

public:
	//固有のパラメータ
	struct Parameter {
		//カメラのprojectionInverse行列
		Matrix4x4 projectionInverse;
	};

public:

	DepthBasedOutline() = default;
	~DepthBasedOutline() = default;

	void Create() override;

	void Render() override;

	void PostRender() override;

	void Debug() override;

	Parameter* parameter_ = nullptr;

private:

	Microsoft::WRL::ComPtr<ID3D12Resource> buffer_;

	/*Microsoft::WRL::ComPtr<ID3D12Resource> depthStencilResource_;*/

	D3D12_CPU_DESCRIPTOR_HANDLE handleCPU_;
	D3D12_GPU_DESCRIPTOR_HANDLE handleGPU_;

};

class RadialBlur : public PostEffects
{

public:
	//固有のパラメータ
	struct Parameter {
		//中心座標
		Vector2 center{};
		//ブラー時の横幅
		float blurWidth = 0.01f;
		//サンプル数
		int32_t numSamples = 1;
	};

public:

	RadialBlur() = default;
	~RadialBlur() = default;

	void Create() override;

	void Render() override;

	void Debug() override;

	Parameter* parameter_ = nullptr;

private:

	Microsoft::WRL::ComPtr<ID3D12Resource> buffer_;

	/*Microsoft::WRL::ComPtr<ID3D12Resource> depthStencilResource_;*/

	D3D12_CPU_DESCRIPTOR_HANDLE handleCPU_;
	D3D12_GPU_DESCRIPTOR_HANDLE handleGPU_;

};

class HSVFilter : public PostEffects
{

public:
	//固有のパラメータ
	struct Parameter {
		//色相
		float hue = 0.0f;
		//彩度
		float saturation = 0.0f;
		//明度
		float value = 0.0f;
	};

public:

	HSVFilter() = default;
	~HSVFilter() = default;

	void Create() override;

	void Render() override;

	void Debug() override;

	Parameter* parameter_ = nullptr;

private:

	Microsoft::WRL::ComPtr<ID3D12Resource> buffer_;

};

class Dissolve : public PostEffects
{

public:
	//固有のパラメータ
	struct Parameter {
		//色相
		Vector3 edgeColor = { 1.0f,1.0f,1.0f };
		//彩度
		float Threshold = 0.5f;
		//明度
		float edgeValue = 0.53f;
	};

public:

	Dissolve() = default;
	~Dissolve() = default;

	void Create() override;

	void Render() override;

	void Debug() override;

	Parameter* parameter_ = nullptr;

private:

	Microsoft::WRL::ComPtr<ID3D12Resource> buffer_;

	Texture* maskTexture_ = nullptr;

};
