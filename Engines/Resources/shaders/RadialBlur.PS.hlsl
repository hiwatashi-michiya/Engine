#include "FullScreen.hlsli"

struct Parameter
{
    float32_t2 center;
    float32_t blurWidth;
    int32_t numSamples;
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
    
    float32_t2 kCenter = float32_t2(0.5f, 0.5f); //中心点
    int32_t kNumSamples = 10; //サンプリング数。多いほど滑らかになるが重い
    float32_t kBlurWidth = 0.01f; //ぼかしの幅。　
    
    //中心から現在のuvに対しての方向を計算
    
    float32_t2 direction = input.texcoord - gParameter.center;
    float32_t3 outputColor = float32_t3(0.0f, 0.0f, 0.0f);
    for (int32_t sampleIndex = 0; sampleIndex < gParameter.numSamples; ++sampleIndex)
    {
        //現在のuvから先程計算した方向にサンプリング点を進めながらサンプリング
        float32_t2 texcoord = input.texcoord + direction * gParameter.blurWidth * float32_t(sampleIndex);
        outputColor.rgb += gTexture.Sample(gSampler, texcoord).rgb;
    }
    
    //平均化する
    outputColor.rgb *= rcp(gParameter.numSamples);
    
    output.color.rgb = outputColor;
    output.color.a = 1.0f;
    return output;
}