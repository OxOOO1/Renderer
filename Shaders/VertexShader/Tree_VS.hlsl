
#include "GlobalCBuffer.hlsl"

cbuffer CBuf : register(b1)
{
    matrix model[250];
    
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
    bool isBillboard : IsBillboard;
    float lightInstensity : LightIntensity;
};


cbuffer DescCBuffer : register(b3)
{
    float3 windPosition;
    float globalTime;
    uint isLeaves;
    uint isBillBoard;
    float globalLightIntensity_VS;
}

Texture2D windTexture : register(t4);
Texture2D windWeightTexture : register(t5);

SamplerState samplerState;



VS_out main(float3 pos : Position, float2 tex : TexCoord, float3 n : Normal, float3 tan : Tangent, float3 bitan : Bitangent, uint instanceID : SV_InstanceID)
{
    VS_out vso;
    
    float weight = pos.y;
    
    vso.pos = mul(float4(pos, 1.0f), model[instanceID]);
    vso.isBillboard = isBillBoard;
    if(!isBillBoard)
    {
        float3 windDir = vso.pos.xyz - windPosition.xyz;
        float windStrength = length(windDir);
        windDir = windDir.xyz / windStrength;
    
        float windOffset = windTexture.SampleLevel(samplerState, globalTime / 200.f, 0).r;
        vso.pos.xz += windDir.xz * windOffset * pow(weight, 1.3) / windStrength * 75.f;
    
        if (isLeaves)
        {
            float leavesWeight = windWeightTexture.SampleLevel(samplerState, tex.xy, 0).r;
            leavesWeight = max(0.f, leavesWeight - 0.89f);
        
            if (leavesWeight > 0.1f)
            {
                float windOffset2 = windTexture.SampleLevel(samplerState, globalTime / windStrength * 500, 0).r;
                vso.pos.xyz += windDir.xyz * windOffset2 * 10 * leavesWeight / windOffset;
                vso.pos.y += windOffset2 * 250 * leavesWeight;
            }
        }
        
    }
    else
    {
        vso.lightInstensity = globalLightIntensity_VS;
    }
    
    vso.pos = mul(vso.pos, camViewProjection);
    vso.tex = tex;
    vso.fragPosLightSpace = mul(float4(pos, 1.0f), model[instanceID]);
    vso.fragPosLightSpace = mul(vso.fragPosLightSpace, globalLightVP);
    matrix modelView = mul(model[instanceID], camView);
    vso.viewPos = (float3) mul(float4(pos, 1.0f), modelView);
    vso.normal = mul(n, (float3x3) modelView);
    vso.tan =    mul(tan, (float3x3) modelView);
    vso.bitan =  mul(bitan, (float3x3) modelView);
    return vso;
}
