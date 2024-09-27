#include "Object3d.hlsli"

struct Material {
	
    float32_t4 color;
    
	int32_t enableLighting;
    float32_t shininess;
    
    //dissolve
    float32_t Threshold;
    float32_t edgeValue;
    float32_t3 edgeColor;
    
    float32_t4x4 uvTransform;
    
};

struct DirectionalLight {
	float32_t4 color;
	float32_t3 direction;
	float intensity;
};

struct PointLight
{
    float32_t4 color; //ライトの色
    float32_t3 position; //ライトの位置
    float intensity; //輝度
    float radius; //ライトの届く最大距離
    float decay; //減衰率
};

struct Camera {
    float32_t3 worldPosition;
};

ConstantBuffer<Material> gMaterial : register(b0);
ConstantBuffer<DirectionalLight> gDirectionalLight : register(b1);
ConstantBuffer<Camera> gCamera : register(b2);
ConstantBuffer<PointLight> gPointLight : register(b3);

Texture2D<float32_t4> gTexture : register(t0);
Texture2D<float32_t> gMaskTexture : register(t1);
SamplerState gSampler : register(s0);

struct PixelShaderOutput {
	float32_t4 color : SV_TARGET0;
};

PixelShaderOutput main(VertexShaderOutput input) {
	PixelShaderOutput output;
	float4 transformedUV = mul(float32_t4(input.texcoord, 0.0f, 1.0f), gMaterial.uvTransform);
	float32_t4 textureColor = gTexture.Sample(gSampler, transformedUV.xy);
    
	//textureのα値が0.5以下のときにPixelを棄却
    if (textureColor.a <= 0.5)
    {
        discard;
    }
    
    //dissolve : mask
    float32_t mask = gMaskTexture.Sample(gSampler, input.texcoord);
    
    if (mask <= gMaterial.Threshold)
    {
        discard;
    }
    
	if (gMaterial.enableLighting != 0) { //Lightingする場合
        
        float32_t3 diffuseDirectionalLight;
        float32_t3 specularDirectionalLight;
        float32_t3 diffusePointLight;
        float32_t3 specularPointLight;
        
        {
            
            //half lambert
            float NdotL = dot(normalize(input.normal), -gDirectionalLight.direction);
            float cos = pow(NdotL * 0.5f + 0.5f, 2.0f);
        
            float32_t3 toEye = normalize(gCamera.worldPosition - input.worldPosition);
    
            //float32_t3 reflectLight = reflect(gDirectionalLight.direction, normalize(input.normal));
        
            float32_t3 halfVector = normalize(-gDirectionalLight.direction + toEye);
        
            float NdotH = dot(normalize(input.normal), halfVector);
            float specularPow = pow(saturate(NdotH), gMaterial.shininess);
    
            //拡散反射
            diffuseDirectionalLight =
            gMaterial.color.rgb * textureColor.rgb * gDirectionalLight.color.rgb * cos * gDirectionalLight.intensity;
        
            //鏡面反射
            specularDirectionalLight =
            gDirectionalLight.color.rgb * gDirectionalLight.intensity * specularPow * float32_t3(1.0f, 1.0f, 1.0f);
            
        }
        
        {
            
            float32_t3 pointLightDirection = normalize(input.worldPosition - gPointLight.position);
            
            float32_t distance = length(gPointLight.position - input.worldPosition); //点光源への距離
            float32_t factor = pow(saturate(-distance / gPointLight.radius + 1.0), gPointLight.decay); //指数によるコントロール
            
            //half lambert
            float NdotL = dot(normalize(input.normal), -pointLightDirection);
            float cos = pow(NdotL * 0.5f + 0.5f, 2.0f);
        
            float32_t3 toEye = normalize(gCamera.worldPosition - input.worldPosition);
    
            //float32_t3 reflectLight = reflect(gDirectionalLight.direction, normalize(input.normal));
        
            float32_t3 halfVector = normalize(-pointLightDirection + toEye);
        
            float NdotH = dot(normalize(input.normal), halfVector);
            float specularPow = pow(saturate(NdotH), gMaterial.shininess);
    
            //拡散反射
            diffusePointLight =
            gMaterial.color.rgb * textureColor.rgb * gPointLight.color.rgb * cos * gPointLight.intensity * factor;
        
            //鏡面反射
            specularPointLight =
            gPointLight.color.rgb * gPointLight.intensity * specularPow * float32_t3(1.0f, 1.0f, 1.0f) * factor;
            
        }
        
        //拡散反射+鏡面反射
        output.color.rgb = diffuseDirectionalLight + specularDirectionalLight + diffusePointLight + specularPointLight;
        output.color.a = gMaterial.color.a * textureColor.a;
    }
	else { //Lightingしない場合
		output.color = gMaterial.color * textureColor;
	}
    
    //dissolve : edge
    float32_t edge = 1.0f - smoothstep(gMaterial.Threshold, gMaterial.Threshold + 0.03f, mask);
    //Edgeに近いほど指定した色を加算
    output.color.rgb += edge * gMaterial.edgeColor;
    
	//output.colorのα値が0のときにPixelを棄却
    if (output.color.a == 0.0)
    {
        discard;
    }
	
	return output;
}
