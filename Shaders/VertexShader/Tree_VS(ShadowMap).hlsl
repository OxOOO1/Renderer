
#include "GlobalCBuffer.hlsl"

cbuffer CBuf : register(b1)
{
    matrix model[100];
    
};

struct VS_out
{
    float2 tex : TexCoord;
    float4 pos : SV_Position;
};


cbuffer DescCBuffer : register(b3)
{
    float3 windPosition;
    float globalTime;
    uint isLeaves;
}

Texture2D windTexture : register(t4);
Texture2D windWeightTexture : register(t5);

SamplerState samplerState;



VS_out main(float3 pos : Position, float2 tex : TexCoord, uint instanceID : SV_InstanceID)
{
    VS_out vso;
    vso.tex = tex;
    
    float weight = pos.y;
    
    vso.pos = mul(float4(pos, 1.0f), model[instanceID]);
    
    float3 windPos = windPosition;
    
    float3 windDir = vso.pos.xyz - windPos.xyz;
    float windStrength = length(windDir);
    windDir = windDir.xyz / windStrength;
    
    float windOffset = windTexture.SampleLevel(samplerState, globalTime / 200.f, 0).r;
    vso.pos.xz += windDir.xz * windOffset * pow(weight, 1.3) / windStrength * 75.f;
    
    vso.pos = mul(vso.pos, globalLightVP);
   
    return vso;
}
