
#include "GlobalCBuffer.hlsl"

struct VS_out
{
    float3 viewPos : Position;
    float3 normal : Normal;
    float3 tan : Tangent;
    float3 bitan : Bitangent;
    float2 tex : TexCoord;
    float4 fragPosLightSpace : fragPosLightSpace;
    float4 pos : SV_Position;
};

cbuffer transform : register(b1)
{
    matrix modelTransforms[100];
};

VS_out main(float4 pos : Position, float2 tex : TexCoord, float3 n : Normal, float3 tan : Tangent, float3 bitan : Bitangent )
{
    VS_out vso;
    vso.tex = tex;
    
    int index = pos.w;
    matrix model = modelTransforms[index];
    float3 worldPos = mul(float4(pos.xyz, 1.0f), model);
    
    vso.pos = mul(float4(worldPos, 1.0f), camViewProjection);
    vso.fragPosLightSpace = mul(float4(worldPos, 1.0f), globalLightVP);
    matrix view = mul(model, camView);
    vso.viewPos = (float3) mul(float4(pos.xyz, 1.0f), view);
    vso.normal = mul(n, (float3x3) view);
    vso.tan = mul(tan, (float3x3) view);
    vso.bitan = mul(bitan, (float3x3) view);
    return vso;
}



