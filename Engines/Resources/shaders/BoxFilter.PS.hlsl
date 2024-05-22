#include "FullScreen.hlsli"

struct Parameter
{
    int32_t size;
};

ConstantBuffer<Parameter> gParameter : register(b0);
Texture2D<float32_t4> gTexture : register(t0);
SamplerState gSampler : register(s0);

static const float32_t2 kIndex3x3[3][3] =
{
    { { -1.0f, -1.0f }, { 0.0f, -1.0f }, { 1.0f, -1.0f } },
    { { -1.0f, 0.0f }, { 0.0f, 0.0f }, { 1.0f, 0.0f } },
    { { -1.0f, 1.0f }, { 0.0f, 1.0f }, { 1.0f, 1.0f } },
};

static const float32_t kKernel3x3[3][3] =
{
    { 1.0f / 9.0f, 1.0f / 9.0f, 1.0f / 9.0f },
    { 1.0f / 9.0f, 1.0f / 9.0f, 1.0f / 9.0f },
    { 1.0f / 9.0f, 1.0f / 9.0f, 1.0f / 9.0f },
};

struct PixelShaderOutput
{
    float32_t4 color : SV_TARGET0;
};

PixelShaderOutput main(VertexShaderOutput input)
{
    PixelShaderOutput output;
    
    uint32_t width, height; //uvStepSizeの算出
    gTexture.GetDimensions(width, height);
    float32_t2 uvStepSize = float32_t2(rcp(width), rcp(height));
    
    output.color.rgb = float32_t3(0.0f, 0.0f, 0.0f);
    output.color.a = 1.0f;
    
    for (int32_t x = -gParameter.size; x <= gParameter.size; ++x){
        
        for (int32_t y = -gParameter.size; y <= gParameter.size; ++y)
        {
            
            //現在のtexcoordを算出
            float32_t2 texcoord =
            {
                clamp(input.texcoord.x + x * uvStepSize.x, 0.0f, width),
                clamp(input.texcoord.y + y * uvStepSize.y, 0.0f, height)
            };
            
            //色に1/9を掛けて足す
            float32_t3 fetchColor = gTexture.Sample(gSampler, texcoord).rgb;
            output.color.rgb += fetchColor * rcp(pow(gParameter.size * 2 + 1, 2));
            
        }
        
    }
    
    return output;
    
}