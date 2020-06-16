
#include "GlobalCBuffer.hlsl"

struct VS_out
{
    float2 tex : TexCoord;
    float4 pos : SV_Position;
};

VS_out main(float3 pos : Position, float2 tex : TexCoord)
{
    VS_out vso;
    vso.tex = tex;
    vso.pos = mul(float4(pos, 1.0f), camViewProjection);
    return vso;
}
