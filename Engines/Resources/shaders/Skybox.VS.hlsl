#include "Skybox.hlsli"

struct SkyboxTransformationMatrix {
	float32_t4x4 WVP;
};

ConstantBuffer<SkyboxTransformationMatrix> gTransformationMatrix : register(b0);

struct VertexShaderInput {
	float32_t4 position : POSITION0;
};

VertexShaderOutput main(VertexShaderInput input) {
	VertexShaderOutput output;
	output.position = mul(input.position, gTransformationMatrix.WVP).xyww;
    output.texcoord = input.position.xyz;
	return output;
}
