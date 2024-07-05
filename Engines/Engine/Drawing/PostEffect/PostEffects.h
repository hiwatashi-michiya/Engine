#pragma once
#include "RenderTexture.h"
#include <string>
#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"
#include "Matrix4x4.h"
#include "Camera.h"

enum PostEffectType {

	kNone, //エフェクト無し
	kGrayscale, //グレースケール
	kVignette, //ビネット
	kBoxFilter, //ぼかし
	kGaussianFilter, //ガウスぼかし
	kLuminanceBasedOutline, //輝度ベースアウトライン
	kDepthBasedOutline, //深度ベースアウトライン
	kRadialBlur, //放射状ブラー

	kMaxEffects, //エフェクト最大数

};

class PostEffects
{
public:

	PostEffects() = default;
	~PostEffects() = default;

	virtual void Create();

	virtual void Render();

	//描画後処理
	virtual void PostRender();

	virtual void Debug();

	void SetCamera(Camera* camera) { camera_ = camera; }

	std::string name_;

	Camera* camera_ = nullptr;

protected:

	ID3D12RootSignature* rootSignature_;

	ID3D12PipelineState* pipelineState_;

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

	struct Parameter {
		Vector3 color = { 1.0f,1.0f,1.0f };
		float colorPower = 0.2f;
		float scale = 16.0f;
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

	struct Parameter {
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

	struct Parameter {
		int32_t size = 2;
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

	struct Parameter {
		Vector2 center{};
		float blurWidth = 0.01f;
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
