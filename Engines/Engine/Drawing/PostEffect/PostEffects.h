#pragma once
#include "RenderTexture.h"
#include <string>
#include "Vector3.h"
#include "Vector4.h"

enum PostEffectType {

	kNone, //エフェクト無し
	kGrayscale, //グレースケール
	kVignette, //ビネット
	kBoxFilter, //ぼかし
	kGaussianFilter, //ガウスぼかし

	kMaxEffects, //エフェクト最大数

};

class PostEffects
{
public:

	PostEffects() = default;
	~PostEffects() = default;

	virtual void Create();

	virtual void Render();

	virtual void Debug();

	std::string name_;

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

private:

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

private:

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

private:

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

private:

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
