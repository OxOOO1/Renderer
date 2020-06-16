

#include "GlobalCBuffer.hlsl"

cbuffer CBuf : register(b1)
{
    matrix model[500];
};


struct VS_out
{
    float2 tex : TexCoord;
    float4 pos : SV_Position;
};


Texture2D windTexture : register(t0);

SamplerState samplerState;

VS_out main(float3 pos : Position, float2 tex : TexCoord, uint instanceID : SV_InstanceID)
{
    
    VS_out vso;
    vso.pos = mul(float4(pos, 1.0f), model[instanceID]);
    vso.pos = mul(vso.pos, globalLightVP);
    vso.tex = tex;
    return vso;
}
