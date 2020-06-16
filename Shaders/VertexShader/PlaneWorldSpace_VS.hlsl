
#include "GlobalCBuffer.hlsl"

struct VS_out
{
    float3 viewPos : Position;
    float3 normal : Normal;
    float3 tan : Tangent;
    float3 bitan : Bitangent;
    float2 tex : TexCoord;
    float4 pos : SV_Position;
    uint instanceID : InstanceID;
};


cbuffer CBuf : register(b2)
{
    matrix model[200];
    
};

cbuffer DescCBuffer : register(b3)
{
    uint numOfInstances;
    uint numOfTextures;
}

VS_out main(float3 pos : Position, float3 n : Normal, float3 tan : Tangent, float3 bitan : Bitangent, float2 tex : TexCoord, uint instanceID : SV_InstanceID)
{
    
    VS_out vso;
    vso.tex = tex;
    vso.pos = mul(float4(pos, 1.0f), model[instanceID]);
    vso.pos = mul(vso.pos, camViewProjection);
    
    matrix modelview = mul(model[instanceID], camView);
    vso.viewPos = (float3) mul(float4(pos, 1.0f), modelview);
    vso.normal = mul(n, (float3x3) modelview);
    vso.tan = mul(tan, (float3x3) modelview);
    vso.bitan = mul(bitan, (float3x3) modelview);
    
    vso.instanceID = instanceID;
    return vso;
}
