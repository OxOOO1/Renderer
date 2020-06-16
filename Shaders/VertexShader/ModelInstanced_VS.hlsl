

#include "GlobalCBuffer.hlsl"

cbuffer CBuf : register(b1)
{
    matrix model[500];
};

struct VS_out
{
    float3 viewPos : Position;
    float3 normal : Normal;
    float3 tan : Tangent;
    float3 bitan : Bitangent;
    float2 tex : TexCoord;
    //float4 projectorPos : ProjectorPos;
    float4 fragPosLightSpace : fragPosLightSpace;
    //float4 worldPos : worldPos;
    float4 pos : SV_Position;
};


VS_out main(float3 pos : Position, float2 tex : TexCoord, float3 n : Normal, float3 tan : Tangent, float3 bitan : Bitangent, uint instanceID : SV_InstanceID)
{
    VS_out vso;
    
    vso.pos = mul(float4(pos, 1.0f), model[instanceID]);
    vso.pos = mul(vso.pos, camViewProjection);
    vso.fragPosLightSpace = mul(float4(pos, 1.0f), model[instanceID]);
    vso.fragPosLightSpace = mul(vso.fragPosLightSpace, globalLightVP);
    matrix modelview = mul(model[instanceID], camView);
    vso.viewPos = (float3) mul(float4(pos, 1.0f), modelview);
    vso.normal = mul(n, (float3x3) modelview);
    vso.tan = mul(tan, (float3x3) modelview);
    vso.bitan = mul(bitan, (float3x3) modelview);
    vso.tex = tex;
    
    return vso;
}
