#include "Functions.hlsl"


cbuffer DescCBuffer : register(b2)
{
    bool hasDiffuse;
    bool hasNormal;
    bool hasSpecular;
    bool hasActiveAlpha;
    bool hasEmitter;
};





Texture2D tex : register(t0);
Texture2D normalTex : register(t1);
Texture2D specTex : register(t2);
Texture2D shadowMap :  register(t3);
Texture2D emitter : register(t4);
//TextureCube omniShadowMap : register(t3);
//Texture2D cookieTex : register(t4);

SamplerState linearSampling : register(s0);
SamplerComparisonState shadowMapSampling : register(s1);

struct VS_in
{
    float3 fragViewPos : Position;
    float3 n : Normal;
    float3 tan : Tangent;
    float3 bitan : Bitangent;
    float2 texCoord : TexCoord;
    //float4 projectorPos : ProjectorPos;
    float4 fragPosLightSpace : fragPosLightSpace;
    //float4 worldPos : worldPos;
    float4 pos : SV_Position;
};

float4 main(VS_in vsin) : SV_Target
{
    float4 Diffuse = float4(0.5f, 0.5f, 0.5f, 1.f);
    
    float3 normal = normalize(vsin.n);
    
    if(numOfPointLights < 1)
    {
        return float4(0.3, 0.3, 0.3, 1.0);
    }
    
    if (hasDiffuse)
    {
        Diffuse = tex.Sample(linearSampling, vsin.texCoord);
        if(hasActiveAlpha){
            clip(Diffuse.a - 0.25f);
        }   
    }
    if (hasNormal)
    {
        normal = CalculateNormal(vsin.tan, vsin.bitan, vsin.n, float3(normalTex.Sample(linearSampling, vsin.texCoord).xy, 1.0f));
    }
    if (hasActiveAlpha)
    {
        Diffuse.rgb = pow(Diffuse.rgb, 2.2);
        if (dot(normal, vsin.fragViewPos) >= 0.0f)
        {
            normal = -normal;
        }
    }
    
    float specularColorIntensity = 0.5f;
    float specularPower = 64.f;
    if (hasSpecular)
    {
        specularColorIntensity = specTex.Sample(linearSampling, vsin.texCoord).r;
    }
    
    float3 directLightColor;
    if(hasActiveAlpha)
    directLightColor = CalculateGlobalLightColorSpecular(Diffuse.rgb, normal, vsin.fragViewPos,specularColorIntensity * 0.25f, specularPower);
    else
    directLightColor = CalculateGlobalLightColor(Diffuse.rgb, normal, vsin.fragViewPos);
    
    
    vsin.fragPosLightSpace.xyz /= vsin.fragPosLightSpace.w;
    directLightColor *= CalculateGlobalLightShadow(vsin.fragPosLightSpace.xyz, shadowMap, shadowMapSampling);
    

    if(numOfPointLights > 0)
    {
        for (int i = 0; i < numOfPointLights; i++)
        {
            directLightColor += CalculatePointLightColor(i, vsin.fragViewPos, Diffuse.rgb, normal, specularColorIntensity, specularPower);
        }
    }
    
    if (numOfSpotLights > 0)
    {
        for (int i = 0; i < numOfSpotLights; i++)
        {
            directLightColor += CalculateSpotlightColorWithCutoff(i, vsin.fragViewPos, Diffuse.rgb, normal, specularColorIntensity, specularPower);
        }
    }
    
    if(hasEmitter)
    {
        directLightColor += emitter.Sample(linearSampling, vsin.texCoord);
    }
    
    return float4(directLightColor, Diffuse.a);
    
}