

#include "GlobalCBuffer.hlsl"

cbuffer CBuf : register(b1)
{
    matrix model[1000];
};

cbuffer DescCBuffer : register(b3)
{
    float3 windPosition;
    float globalTime;
}

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
    
    float weight = pos.y;
    
    vso.pos = mul(float4(pos, 1.0f), model[instanceID]);
    
    float3 windDir = vso.pos.xyz - windPosition;
    float windStrength = length(windDir);
    windDir = windDir.xyz / windStrength;
    windStrength /= 500.f;
    
    float windOffset = windTexture.SampleLevel(samplerState, globalTime / 500, 0).r;
        
    float minY = vso.pos.y - vso.pos.y / 4;
    vso.pos.xyz += windDir.xyz * windOffset * pow(weight, 1.5f) / windStrength;
    vso.pos.y = max(minY, vso.pos.y);
    
    vso.pos = mul(vso.pos, globalLightVP);
    vso.tex = tex;
    return vso;
}
