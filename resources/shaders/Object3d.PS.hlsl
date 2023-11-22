#include "Object3d.hlsli"

struct Material {
	float32_t4 color;
	int32_t enableLighting;
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
	if (gMaterial.enableLighting != 0) { //Lightingする場合
		//half lambert
		float NdotL = dot(normalize(input.normal), -gDirectionalLight.direction);
		float cos = pow(NdotL * 0.5f + 0.5f, 2.0f);
        output.color.rgb = gMaterial.color.rgb * textureColor.rgb * gDirectionalLight.color.rgb * cos * gDirectionalLight.intensity;
        output.color.a = gMaterial.color.a * textureColor.a;
    }
	else { //Lightingしない場合
		output.color = gMaterial.color * textureColor;
	}

    if (output.color.a == 0.0f)
    {
        discard;
    }
	
	return output;
}
