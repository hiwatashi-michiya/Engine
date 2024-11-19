#include "FullScreen.hlsli"

struct Parameter
{
    float32_t3 edgeColor;
    float32_t Threshold;
    float32_t edgeValue;
};

Texture2D<float32_t4> gTexture : register(t0);
Texture2D<float32_t> gMaskTexture : register(t1);
ConstantBuffer<Parameter> gParameter : register(b0);
SamplerState gSampler : register(s0);

struct PixelShaderOutput
{
    float32_t4 color : SV_TARGET0;
};

PixelShaderOutput main(VertexShaderOutput input)
{
    
    float32_t mask = gMaskTexture.Sample(gSampler, input.texcoord);
    
    if (mask <= gParameter.Threshold)
    {
        discard;
    }
    
    PixelShaderOutput output;
    float32_t edge = 1.0f - smoothstep(gParameter.Threshold, gParameter.edgeValue, mask);
    output.color = gTexture.Sample(gSampler, input.texcoord);
    //Edgeに近いほど指定した色を加算
    output.color.rgb += edge * gParameter.edgeColor;
    output.color.a = 1.0f;
    return output;
}