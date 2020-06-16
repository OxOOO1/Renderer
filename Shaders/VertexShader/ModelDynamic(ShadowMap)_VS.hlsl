
#include "GlobalCBuffer.hlsl"

struct VS_out
{
    float2 tex : TexCoord;
    float4 pos : SV_Position;
};

cbuffer transform : register(b1)
{
    matrix modelTransforms[100];
};

VS_out main(float4 pos : Position, float2 tex : TexCoord)
{
    VS_out vso;
    vso.tex = tex;
    
    float index = pos.w;
    
    matrix model = modelTransforms[index];
    
    float3 worldPos = mul(float4(pos.xyz, 1.0f), model);
    vso.pos = mul(float4(worldPos, 1.0f), camViewProjection);
    
    return vso;
}
