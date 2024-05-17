#pragma once
#include "RenderTexture.h"

enum PostEffectType {

	kNone, //エフェクト無し
	kGrayscale, //グレースケール
	kVignette, //ビネット

	kMaxEffects, //エフェクト最大数

};

class PostEffects
{
public:

	PostEffects() = default;
	~PostEffects() = default;

	virtual void Create();

	virtual void Render();

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

	Parameter* parameter_;

private:

	Microsoft::WRL::ComPtr<ID3D12Resource> buffer_;

};

class Vignette : public PostEffects
{

private:

	struct Parameter {

	};

public:

	Vignette() = default;
	~Vignette() = default;

	void Create() override;

	void Render() override;

	Parameter* parameter_;

private:

	Microsoft::WRL::ComPtr<ID3D12Resource> buffer_;

};

class BoxFilter : public PostEffects
{

private:

	struct Parameter {

	};

public:

	BoxFilter() = default;
	~BoxFilter() = default;

	void Create() override;

	void Render() override;

	Parameter* parameter_;

private:

	Microsoft::WRL::ComPtr<ID3D12Resource> buffer_;

};


