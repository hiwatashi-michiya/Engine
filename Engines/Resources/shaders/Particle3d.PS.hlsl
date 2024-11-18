#include "Particle3d.hlsli"

struct Material {
	float32_t4 color;
	int32_t enableLighting;
    float32_t shininess;
    
    //dissolve
    float32_t Threshold;
    float32_t edgeValue;
    float32_t3 edgeColor;
    
    //ノイズ
    int32_t isActiveNoise;
	
	float32_t4x4 uvTransform;
};

struct DirectionalLight {
	float32_t4 color;
	float32_t3 direction;
	float intensity;
};

ConstantBuffer<Material> gMaterial : register(b0);
ConstantBuffer<DirectionalLight> gDirectionalLight : register(b1);

Texture2D<float32_t4> gTexture : register(t0);
SamplerState gSampler : register(s0);

struct PixelShaderOutput {
	float32_t4 color : SV_TARGET0;
};

PixelShaderOutput main(VertexShaderOutput input) {
	PixelShaderOutput output;
	float4 transformedUV = mul(float32_t4(input.texcoord, 0.0f, 1.0f), gMaterial.uvTransform);
	float32_t4 textureColor = gTexture.Sample(gSampler, transformedUV.xy);
	
	//output.colorのα値が0のときにPixelを棄却
    if (output.color.a == 0.0)
    {
        discard;
    }
	
    output.color = gMaterial.color * textureColor * input.color;
	
	return output;
}
