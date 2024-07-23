#include "FullScreen.hlsli"

struct Parameter
{
    float32_t hue;
    float32_t saturation;
    float32_t value;
};

ConstantBuffer<Parameter> gParameter : register(b0);
Texture2D<float32_t4> gTexture : register(t0);
SamplerState gSampler : register(s0);

float32_t3 RGBToHSV(float32_t3 rgb)
{
    
    float32_t3 outputHSV;
    
    float32_t max = rgb.r > rgb.g ? rgb.r : rgb.g;
    max = max > rgb.b ? max : rgb.b;
    float32_t min = rgb.r < rgb.g ? rgb.r : rgb.g;
    min = min < rgb.b ? min : rgb.b;
    float32_t h = max - min;
    
    if (h > 0.0f)
    {
        
        if (max == rgb.r)
        {
            
            h = (rgb.g - rgb.b) / h;
            
            if (h < 0.0f)
            {
                h += 6.0f;
            }
            
        }
        else if (max == rgb.g)
        {
            h = 2.0f + (rgb.b - rgb.r) / h;
        }
        else
        {
            h = 4.0f + (rgb.r - rgb.g) / h;
        }
        
    }
    
    h /= 6.0f;
    
    float s = (max - min);
    
    if (max != 0.0f)
    {
        s /= max;
    }
    
    float v = max;
    
    outputHSV.x = h;
    outputHSV.y = s;
    outputHSV.z = v;
    
    return outputHSV;
    
}

float32_t3 HSVToRGB(float32_t3 hsv)
{
    
    float32_t3 outputRGB;
    
    outputRGB.r = hsv.z;
    outputRGB.g = hsv.z;
    outputRGB.b = hsv.z;
    
    if (hsv.y > 0.0f)
    {
        
        hsv.x *= 6.0f;
        const int i = (int) hsv.x;
        const float f = hsv.x - (float) i;
        
        if (i == 0 || i > 5)
        {
            outputRGB.g *= 1.0f - hsv.y * (1.0f - f);
            outputRGB.b *= 1.0f - hsv.y;
        }
        else if (i == 1)
        {
            outputRGB.r *= 1.0f - hsv.y * f;
            outputRGB.b *= 1.0f - hsv.y;
        }
        else if (i == 2)
        {
            outputRGB.r *= 1.0f - hsv.y;
            outputRGB.b *= 1.0f - hsv.y * (1.0f - f);
        }
        else if (i == 3)
        {
            outputRGB.r *= 1.0f - hsv.y;
            outputRGB.g *= 1.0f - hsv.y * f;
        }
        else if (i == 4)
        {
            outputRGB.r *= 1.0f - hsv.y * (1.0f - f);
            outputRGB.g *= 1.0f - hsv.y;
        }
        else if (i == 5)
        {
            outputRGB.g *= 1.0f - hsv.y;
            outputRGB.b *= 1.0f - hsv.y * f;
        }
        
    }
    
    return outputRGB;
    
}

float32_t WrapValue(float32_t value, float32_t minRange, float32_t maxRange)
{
    
    float32_t range = maxRange - minRange;
    float32_t modValue = fmod(value - minRange, range);
    
    if (modValue < 0.0f)
    {
        modValue += range;
    }
    
    return minRange + modValue;
    
}

struct PixelShaderOutput
{
    float32_t4 color : SV_TARGET0;
};

PixelShaderOutput main(VertexShaderOutput input)
{
    PixelShaderOutput output;
    output.color = gTexture.Sample(gSampler, input.texcoord);
    
    float32_t3 hsv = RGBToHSV(output.color.rgb);
    
    hsv.x += gParameter.hue;
    hsv.y += gParameter.saturation;
    hsv.z += gParameter.value;
    
    hsv.x = WrapValue(hsv.x, 0.0f, 1.0f);
    hsv.y = saturate(hsv.y);
    hsv.z = saturate(hsv.z);
    
    float32_t3 rgb = HSVToRGB(hsv);
    
    output.color.rgb = rgb;
    
    return output;
}