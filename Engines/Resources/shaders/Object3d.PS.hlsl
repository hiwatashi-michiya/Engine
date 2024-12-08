#include "Object3d.hlsli"
#include "Material3d.hlsli"

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

float32_t random(float32_t2 value)
{
    
    return frac(sin(dot(value, float32_t2(12.9898f, 78.233f))) * 143758.5453f);
    
}

float32_t2 randomVec(float32_t2 fact)
{
    
    float32_t2 angle = float32_t2(
    dot(fact, float32_t2(127.1f, 311.7f)),
    dot(fact, float32_t2(269.5f, 183.3f))
    );
    
    return frac(sin(angle) * 43758.545313f) * 2 - 1;
    
}

float32_t PerlinNoise(float32_t density, float32_t2 uv)
{
    
    float32_t2 uvFloor = floor(uv * density);
    float32_t2 uvFrac = frac(uv * density);
    
    //各頂点のランダムなベクトルを取得
    float32_t2 v00 = randomVec(uvFloor + float32_t2(0, 0));
    float32_t2 v01 = randomVec(uvFloor + float32_t2(0, 1));
    float32_t2 v10 = randomVec(uvFloor + float32_t2(1, 0));
    float32_t2 v11 = randomVec(uvFloor + float32_t2(1, 1));
    
    //内積を取る
    float32_t c00 = dot(v00, uvFrac - float32_t2(0, 0));
    float32_t c01 = dot(v01, uvFrac - float32_t2(0, 1));
    float32_t c10 = dot(v10, uvFrac - float32_t2(1, 0));
    float32_t c11 = dot(v11, uvFrac - float32_t2(1, 1));
    
    float32_t2 u = uvFrac * uvFrac * (3 - 2 * uvFrac);
    
    float32_t v0010 = lerp(c00, c10, u.x);
    float32_t v0111 = lerp(c01, c11, u.x);
    
    return lerp(v0010, v0111, u.y) / 2 + 0.5f;
    
}

float32_t FractalSumNoise(float32_t density, float32_t2 uv)
{
    float32_t fn;
    fn = PerlinNoise(density * 1, uv) * 1.0f / 2;
    fn += PerlinNoise(density * 2, uv) * 1.0f / 4;
    fn += PerlinNoise(density * 4, uv) * 1.0f / 8;
    fn += PerlinNoise(density * 8, uv) * 1.0f / 16;
    return fn;
}

PixelShaderOutput main(VertexShaderOutput input) {
	PixelShaderOutput output;
	float4 transformedUV = mul(float32_t4(input.texcoord, 0.0f, 1.0f), gMaterial.uvTransform);
	float32_t4 textureColor = gTexture.Sample(gSampler, transformedUV.xy);
    
    float32_t density = 20.0f;
    float32_t pn = 1.0f;
    
    float32_t4 usingColor = gMaterial.color;
    
    //ノイズを利用する場合
    if (gMaterial.isActiveNoise != 0)
    {
        pn = FractalSumNoise(density, input.texcoord);
    }
    
	//textureのα値が0.5以下のときにPixelを棄却
    if (textureColor.a <= 0.1)
    {
        discard;
    }
    
    //dissolve : mask
    float32_t mask = gMaskTexture.Sample(gSampler, input.texcoord);
    
    if (mask <= gMaterial.Threshold)
    {
        usingColor = gMaterial.secondColor;
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
            usingColor.rgb * textureColor.rgb * gDirectionalLight.color.rgb * cos * gDirectionalLight.intensity;
        
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
            usingColor.rgb * textureColor.rgb * gPointLight.color.rgb * cos * gPointLight.intensity * factor;
        
            //鏡面反射
            specularPointLight =
            gPointLight.color.rgb * gPointLight.intensity * specularPow * float32_t3(1.0f, 1.0f, 1.0f) * factor;
            
        }
        
        //拡散反射+鏡面反射
        output.color.rgb = (diffuseDirectionalLight + specularDirectionalLight + diffusePointLight + specularPointLight) * pn;
        output.color.a = usingColor.a * textureColor.a;
    }
	else { //Lightingしない場合
        output.color = usingColor * textureColor;
    }
    
    if (mask <= gMaterial.Threshold + 0.03f
        && mask >= gMaterial.Threshold)
    {
      
        //dissolve : edge
        float32_t edge = 1.0f - smoothstep(gMaterial.Threshold, gMaterial.Threshold + 0.03f, mask);
        //Edgeに近いほど指定した色を加算
        output.color.rgb += edge * gMaterial.edgeColor;
        
    }
    
	//output.colorのα値が0のときにPixelを棄却
    if (output.color.a == 0.0)
    {
        discard;
    }
	
	return output;
}
