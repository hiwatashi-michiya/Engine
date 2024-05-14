#include "FullScreen.hlsli"

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
    float vignette = correct.x * correct.y * 16.0f;
    //n乗する
    vignette = saturate(pow(vignette, 0.8f));
    //係数として乗算
    output.color.rgb *= vignette;
    
    return output;
}