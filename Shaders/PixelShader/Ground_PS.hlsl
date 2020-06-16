#include "Functions.hlsl"

Texture2DArray terrainTexDiff : register(t0);
Texture2DArray terrainTexNorm : register(t1);

Texture2D shadowMap :  register(t3);

SamplerState linearSampling : register(s0);
SamplerComparisonState shadowMapSampling : register(s1);

struct VS_in
{
    float texWeight : TexWeight;
    float3 fragViewPos : Position;
    float3 n : Normal;
    float3 tan : Tangent;
    float3 bitan : Bitangent;
    float2 texCoord : TexCoord;
    //float4 projectorPos : ProjectorPos;
    float4 fragPosLightSpace : fragPosLightSpace;
    //float4 worldPos : worldPos;
    //float4 pos : SV_Position;
};

float4 main(VS_in vsin) : SV_Target
{
    vsin.fragPosLightSpace.xyz /= vsin.fragPosLightSpace.w;
    
    vsin.n = normalize(vsin.n);
    
    float4 Diffuse1 = terrainTexDiff.Sample(linearSampling, float3(vsin.texCoord.xy, 0));
    float4 Diffuse2 = terrainTexDiff.Sample(linearSampling, float3(vsin.texCoord.xy, 1));
    
    float3 normal1 = CalculateNormal(vsin.tan, vsin.bitan, vsin.n, float3(terrainTexNorm.Sample(linearSampling, float3(vsin.texCoord.xy, 0)).xy, 1.0f));
    float3 normal2 = CalculateNormal(vsin.tan, vsin.bitan, vsin.n, float3(terrainTexNorm.Sample(linearSampling, float3(vsin.texCoord.xy, 1)).xy, 1.0f));
    
    float4 Diffuse = Diffuse2 * vsin.texWeight + Diffuse1 * (1.0 - vsin.texWeight);
    float3 normal = normal2 * vsin.texWeight + normal1 * (1.0 - vsin.texWeight);
    
    float specularPower = 32.f;
    float specularColorIntensity = Diffuse.a;
    
    float3 directLightColor = CalculateGlobalLightColor(Diffuse.rgb, normal, vsin.fragViewPos) * 0.5f;
    
    directLightColor *= CalculateGlobalLightShadow(vsin.fragPosLightSpace.xyz, shadowMap, shadowMapSampling);
    
    if (numOfPointLights > 0)
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
    
    return float4(directLightColor, 1.0f);
    
}