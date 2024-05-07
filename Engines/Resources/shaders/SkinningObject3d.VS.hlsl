#include "Object3d.hlsli"

struct TransformationMatrix {
	float32_t4x4 WVP;
	float32_t4x4 World;
    float32_t4x4 WorldInverseTranspose;
};

struct Well
{
    float32_t4x4 skeletonSpaceMatrix;
    float32_t4x4 skeletonSpaceInverseTransposeMatrix;
};

ConstantBuffer<TransformationMatrix> gTransformationMatrix : register(b0);
StructuredBuffer<Well> gMatrixPalette : register(t0);

struct VertexShaderInput {
	float32_t4 position : POSITION0;
	float32_t2 texcoord : TEXCOORD0;
	float32_t3 normal : NORMAL0;
    float32_t4 weight : WEIGHT0;
    int32_t4 index : INDEX0;
};

struct Skinned
{
    float32_t4 position;
    float32_t3 normal;
};

Skinned Skinning(VertexShaderInput input)
{
    
	Skinned skinned;
	
	//Skinningの処理
    
    //位置の変換
    skinned.position = mul(input.position, gMatrixPalette[input.index.x].skeletonSpaceMatrix) * input.weight.x;
    skinned.position += mul(input.position, gMatrixPalette[input.index.y].skeletonSpaceMatrix) * input.weight.y;
    skinned.position += mul(input.position, gMatrixPalette[input.index.z].skeletonSpaceMatrix) * input.weight.z;
    skinned.position += mul(input.position, gMatrixPalette[input.index.w].skeletonSpaceMatrix) * input.weight.w;
    skinned.position.w = 1.0f; //確実に1を入れる
	
    //法線の変換
    skinned.normal = mul(input.normal, (float32_t3x3) gMatrixPalette[input.index.x].skeletonSpaceInverseTransposeMatrix) * input.weight.x;
    skinned.normal += mul(input.normal, (float32_t3x3) gMatrixPalette[input.index.y].skeletonSpaceInverseTransposeMatrix) * input.weight.y;
    skinned.normal += mul(input.normal, (float32_t3x3) gMatrixPalette[input.index.z].skeletonSpaceInverseTransposeMatrix) * input.weight.z;
    skinned.normal += mul(input.normal, (float32_t3x3) gMatrixPalette[input.index.w].skeletonSpaceInverseTransposeMatrix) * input.weight.w;
    skinned.normal = normalize(skinned.normal); //正規化して戻す
    
    return skinned;
	
}

VertexShaderOutput main(VertexShaderInput input) {
	VertexShaderOutput output;
    
    //Skinning計算を行って、Skinning後の頂点情報を手に入れる。ここでの頂点もSkeletonSpace
    Skinned skinned = Skinning(input);
    
    //Skinning結果を使って変換
	output.position = mul(skinned.position, gTransformationMatrix.WVP);
    output.worldPosition = mul(skinned.position, gTransformationMatrix.World).xyz;
	output.texcoord = input.texcoord;
    output.normal = normalize(mul(skinned.normal, (float32_t3x3) gTransformationMatrix.WorldInverseTranspose));
	return output;
}
