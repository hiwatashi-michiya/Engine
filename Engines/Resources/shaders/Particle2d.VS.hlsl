#include "Particle2d.hlsli"

struct ParticleForGPU {
	float32_t4x4 WVP;
    float32_t2 texcoords[4];
    float32_t4 color;
    float32_t2 size;
};

StructuredBuffer<ParticleForGPU> gParticle : register(t0);

struct VertexShaderInput {
	float32_t4 position : POSITION0;
	float32_t2 texcoord : TEXCOORD0;
};

VertexShaderOutput main(VertexShaderInput input, uint32_t instanceId : SV_InstanceID, uint32_t vertexId : SV_VertexID)
{
    VertexShaderOutput output;]
    float32_t4 pos = input.position;
    pos.xy = pos.xy * gParticle[instanceId].size;
    output.position = mul(pos, gParticle[instanceId].WVP);
    output.texcoord = gParticle[instanceId].texcoords[vertexId];
    output.color = gParticle[instanceId].color;
	return output;
}
