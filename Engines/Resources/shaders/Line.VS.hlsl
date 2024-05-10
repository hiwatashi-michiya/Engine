#include "Line.hlsli"

struct wvpMatrix
{
    float32_t4x4 mat;
};

ConstantBuffer<wvpMatrix> gWvpMatrix : register(b0);

struct VertexShaderInput
{
    float32_t4 position : POSITION0;
    float32_t4 color : COLOR0;
};

VertexShaderOutput main(VertexShaderInput input)
{
	
    VertexShaderOutput output;
	
    output.position = mul(input.position, gWvpMatrix.mat);
    output.color = input.color;
    
    return output;
	
}