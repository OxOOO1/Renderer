

#include "GlobalCBuffer.hlsl"

struct VS_out
{
    float3 viewPos : Position;
    float3 normal : Normal;
    float3 tan : Tangent;
    float3 bitan : Bitangent;
    float2 tex : TexCoord;
    float4 pos : SV_Position;
    
};

cbuffer DescCBuffer : register(b2)
{
    uint numOfInstances;
    uint numOfTextures;
}

VS_out main(float3 pos : Position, float3 n : Normal, float3 tan : Tangent, float3 bitan : Bitangent, float2 tex : TexCoord)
{
    VS_out vso;
    vso.tex = tex;
    vso.pos = mul(float4(pos, 1.0f), camViewProjection);
    vso.viewPos = (float3) mul(float4(pos, 1.0f), camView);
    vso.normal = mul(n, (float3x3) camView);
    vso.tan = mul(tan, (float3x3) camView);
    vso.bitan = mul(bitan, (float3x3) camView);
    return vso;
}
