#include "FullScreen.hlsli"

struct Parameter
{
    float32_t3 color;
    float32_t colorPower;
    float32_t scale;
    float32_t power;
};

ConstantBuffer<Parameter> gParameter : register(b0);
Texture2D<float32_t4> gTexture : register(t0);
SamplerState gSampler : register(s0);

struct PixelShaderOutput
{
    float32_t4 color : SV_TARGET0;
};

PixelShaderOutput main(VertexShaderOutput input)
{
    PixelShaderOutput output;
    
    output.color = gTexture.Sample(gSampler, input.texcoord);
    
    //周囲を0に、中心になるほど明るくなるように計算で調整
    float32_t2 correct = input.texcoord * (1.0f - input.texcoord.yx);
    //中心の最大値をスケールで調整
    float vignette = correct.x * correct.y * gParameter.scale;
    //n乗する
    vignette = saturate(pow(vignette, gParameter.power));
    //係数として乗算
    output.color.r *= vignette + (gParameter.color.r - vignette) * gParameter.colorPower;
    output.color.g *= vignette + (gParameter.color.g - vignette) * gParameter.colorPower;
    output.color.b *= vignette + (gParameter.color.b - vignette) * gParameter.colorPower;
    
    return output;
}