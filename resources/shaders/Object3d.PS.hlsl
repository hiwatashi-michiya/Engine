#include "Object3d.hlsli"

struct Material {
	
    float32_t4 color;
    
	int32_t enableLighting;
	
    float32_t4x4 uvTransform;
    
    float32_t shininess;
    int32_t isGrayScale;
    int32_t isInversion;
    int32_t isRetro;
    
    int32_t isAverageBlur;
    int32_t isEmboss;
    int32_t isSharpness;
    int32_t isOutline;
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
	
	//平均化ブラーをかける場合
    if (gMaterial.isAverageBlur != 0)
    {
		//テクスチャのサイズ情報を取得
        float w, h, levels;
        gTexture.GetDimensions(0, w, h, levels);
		
        float dx = 1.0f / w;
        float dy = 1.0f / h;
        float4 ret = float4(0.0f, 0.0f, 0.0f, 0.0f);
		
        ret += gTexture.Sample(gSampler, transformedUV.xy + float2(-2 * dx, -2 * dy)); //左上
        ret += gTexture.Sample(gSampler, transformedUV.xy + float2(0, -2 * dy)); //上
        ret += gTexture.Sample(gSampler, transformedUV.xy + float2(2 * dx, -2 * dy)); //右上
		
        ret += gTexture.Sample(gSampler, transformedUV.xy + float2(-2 * dx, 0)); //左
        ret += gTexture.Sample(gSampler, transformedUV.xy); //現在座標
        ret += gTexture.Sample(gSampler, transformedUV.xy + float2(2 * dx, 0)); //右
        
        ret += gTexture.Sample(gSampler, transformedUV.xy + float2(-2 * dx, 2 * dy)); //左下
        ret += gTexture.Sample(gSampler, transformedUV.xy + float2(0, 2 * dy)); //下
        ret += gTexture.Sample(gSampler, transformedUV.xy + float2(2 * dx, 2 * dy)); //右下
		
        textureColor = ret / 9.0f;
        
    }
	
    //エンボス
    if (gMaterial.isEmboss != 0)
    {
		//テクスチャのサイズ情報を取得
        float w, h, levels;
        gTexture.GetDimensions(0, w, h, levels);
		
        float dx = 1.0f / w;
        float dy = 1.0f / h;
        float4 ret = float4(0.0f, 0.0f, 0.0f, 0.0f);
		
        ret += gTexture.Sample(gSampler, transformedUV.xy + float2(-2 * dx, -2 * dy)) * 2; //左上
        ret += gTexture.Sample(gSampler, transformedUV.xy + float2(0, -2 * dy)); //上
        ret += gTexture.Sample(gSampler, transformedUV.xy + float2(2 * dx, -2 * dy)) * 0; //右上
		
        ret += gTexture.Sample(gSampler, transformedUV.xy + float2(-2 * dx, 0)); //左
        ret += gTexture.Sample(gSampler, transformedUV.xy); //現在座標
        ret += gTexture.Sample(gSampler, transformedUV.xy + float2(2 * dx, 0)) * -1; //右
        
        ret += gTexture.Sample(gSampler, transformedUV.xy + float2(-2 * dx, 2 * dy)) * 0; //左下
        ret += gTexture.Sample(gSampler, transformedUV.xy + float2(0, 2 * dy)) * -1; //下
        ret += gTexture.Sample(gSampler, transformedUV.xy + float2(2 * dx, 2 * dy)) * -2; //右下
		
        textureColor = ret;
        
    }
    
    //シャープネス
    if (gMaterial.isSharpness != 0)
    {
		//テクスチャのサイズ情報を取得
        float w, h, levels;
        gTexture.GetDimensions(0, w, h, levels);
		
        float dx = 1.0f / w;
        float dy = 1.0f / h;
        float4 ret = float4(0.0f, 0.0f, 0.0f, 0.0f);
		
        ret += gTexture.Sample(gSampler, transformedUV.xy + float2(-2 * dx, -2 * dy)) * 0; //左上
        ret += gTexture.Sample(gSampler, transformedUV.xy + float2(0, -2 * dy)) * -1; //上
        ret += gTexture.Sample(gSampler, transformedUV.xy + float2(2 * dx, -2 * dy)) * 0; //右上
		
        ret += gTexture.Sample(gSampler, transformedUV.xy + float2(-2 * dx, 0)) * -1; //左
        ret += gTexture.Sample(gSampler, transformedUV.xy) * 5; //現在座標
        ret += gTexture.Sample(gSampler, transformedUV.xy + float2(2 * dx, 0)) * -1; //右
        
        ret += gTexture.Sample(gSampler, transformedUV.xy + float2(-2 * dx, 2 * dy)) * 0; //左下
        ret += gTexture.Sample(gSampler, transformedUV.xy + float2(0, 2 * dy)) * -1; //下
        ret += gTexture.Sample(gSampler, transformedUV.xy + float2(2 * dx, 2 * dy)) * 0; //右下
		
        textureColor = ret;
        
    }
    
    //アウトライン
    if (gMaterial.isOutline != 0)
    {
		//テクスチャのサイズ情報を取得
        float w, h, levels;
        gTexture.GetDimensions(0, w, h, levels);
		
        float dx = 1.0f / w;
        float dy = 1.0f / h;
        float4 ret = float4(0.0f, 0.0f, 0.0f, 0.0f);
		
        ret += gTexture.Sample(gSampler, transformedUV.xy + float2(-2 * dx, -2 * dy)) * 0; //左上
        ret += gTexture.Sample(gSampler, transformedUV.xy + float2(0, -2 * dy)) * -1; //上
        ret += gTexture.Sample(gSampler, transformedUV.xy + float2(2 * dx, -2 * dy)) * 0; //右上
		
        ret += gTexture.Sample(gSampler, transformedUV.xy + float2(-2 * dx, 0)) * -1; //左
        ret += gTexture.Sample(gSampler, transformedUV.xy) * 4; //現在座標
        ret += gTexture.Sample(gSampler, transformedUV.xy + float2(2 * dx, 0)) * -1; //右
        
        ret += gTexture.Sample(gSampler, transformedUV.xy + float2(-2 * dx, 2 * dy)) * 0; //左下
        ret += gTexture.Sample(gSampler, transformedUV.xy + float2(0, 2 * dy)) * -1; //下
        ret += gTexture.Sample(gSampler, transformedUV.xy + float2(2 * dx, 2 * dy)) * 0; //右下
		
        textureColor = ret;
        
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

	//グレースケール化する場合
    if (gMaterial.isGrayScale != 0)
    {
        output.color.rgb = dot(output.color.rgb, float3(0.299f, 0.587f, 0.114f)); //数字はグレースケールの規定値
    }
	
	//色反転する場合
    if (gMaterial.isInversion != 0)
	{
        output.color.rgb = float3(1.0f - output.color.rgb);
    }
	
	//レトロ化する場合
    if (gMaterial.isRetro != 0)
    {
        output.color.rgb = float3(output.color.rgb - fmod(output.color.rgb, 0.25f));
    }
	
	//output.colorのα値が0のときにPixelを棄却
    if (output.color.a == 0.0)
    {
        discard;
    }
	
	return output;
}
