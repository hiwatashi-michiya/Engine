#include "FullScreen.hlsli"

struct Parameter
{
    int32_t size;
};

ConstantBuffer<Parameter> gParameter : register(b0);
Texture2D<float32_t4> gTexture : register(t0);
SamplerState gSampler : register(s0);

static const float32_t PI = 3.14159265f;

float gauss(float x, float y, float sigma)
{
    float exponent = -(x * x + y * y) * rcp(2.0f * sigma * sigma);
    float denominator = 2.0f * PI * sigma * sigma;
    return exp(exponent) * rcp(denominator);
}

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
    
    //kernelを求める
    float32_t weight = 0.0f;
    float32_t kernel[gParameter.size * 2 + 1][gParameter.size * 2 + 1];
    
    for (int32_t x = -gParameter.size; x <= gParameter.size; ++x)
    {
        
        for (int32_t y = -gParameter.size; y <= gParameter.size; ++y)
        {
            kernel[x + gParameter.size][y + gParameter.size];
            
        }
        
    }
    
    for (int32_t x = -gParameter.size; x <= gParameter.size; ++x)
    {
        
        for (int32_t y = -gParameter.size; y <= gParameter.size; ++y)
        {
            
            //現在のtexcoordを算出
            float32_t2 texcoord =
            {
                clamp(input.texcoord.x + x * uvStepSize.x, 0.0f, width),
                clamp(input.texcoord.y + y * uvStepSize.y, 0.0f, height)
            };
            
            //色に参照範囲の逆数を掛けて足す
            float32_t3 fetchColor = gTexture.Sample(gSampler, texcoord).rgb;
            output.color.rgb += fetchColor * rcp(pow(gParameter.size * 2 + 1, 2));
            
        }
        
    }
    
    return output;
    
}