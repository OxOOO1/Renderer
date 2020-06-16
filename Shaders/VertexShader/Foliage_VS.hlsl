

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


Texture2D windTexture : register(t0);

SamplerState samplerState;

VS_out main(float3 pos : Position, float2 tex : TexCoord, float3 n : Normal, float3 tan : Tangent, float3 bitan : Bitangent, uint instanceID : SV_InstanceID)
{
    VS_out vso;
    
    float weight = (abs(pos.x) + pos.y + abs(pos.z)) / 3.f;
    
    
    vso.pos = mul(float4(pos, 1.0f), model[instanceID]);
    
    float3 windDir = vso.pos.xyz - windPosition;
    float windStrength = length(windDir);
    windDir = windDir.xyz / windStrength;
    windStrength /= 500.f;
    
    float windOffset = windTexture.SampleLevel(samplerState, globalTime * windStrength / 250, 0).r;
        
    float minY = vso.pos.y - vso.pos.y / 4;
    vso.pos.xyz += windDir.xyz * windOffset * pow(weight, 1.2f) / 10.f;
    vso.pos.y = max(minY, vso.pos.y);
    
    
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
