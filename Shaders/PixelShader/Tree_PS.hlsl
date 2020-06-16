#include "Functions.hlsl"


cbuffer DescCBuffer : register(b2)
{
    bool hasDiffuse;
    bool hasNormal;
    bool hasSpecular;
    bool hasActiveAlpha;
};

//cbuffer PointLightCBuf : register(b2)
//{
//    float3 pointLightPos;
//    float specularIntensity;
//    float specularPower;
//    float attConst;
//    float attLin;
//    float attQuad;
//    float3 pointLightWorldPos;
//    float pointLightFarPlane;
//};

//cbuffer SpotlightCBuf : register(b3)
//{
//    float4 spotlightDirection;
//    float3 spotlightPosition;
//    float spotlightCutoff;
//    float spotlightInnerCutoff;
//    float spotlightPower;
//    float attConstS;
//    float attLinS;
//    float attQuadS;
//    uint spotlightWidth;
//    uint spotlightHeight;
//}


Texture2D tex : register(t0);
Texture2D normalTex : register(t1);
Texture2D specTex : register(t2);
Texture2D shadowMap :  register(t3);
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
    bool isBillboard : IsBillboard;
    float lightInstensity : LightIntensity;
};

float4 main(VS_in vsin) : SV_Target
{
    
    vsin.n = normalize(vsin.n);
    
    float4 Diffuse = float4(0.5f, 0.5f, 0.5f, 1.f);
    
    float3 normal = vsin.n;
    
    if (hasDiffuse)
    {
        Diffuse = tex.Sample(linearSampling, vsin.texCoord);
        
    }
    if (hasNormal)
    {
        normal = CalculateNormal(vsin.tan, vsin.bitan, vsin.n, float3(normalTex.Sample(linearSampling, vsin.texCoord).xy, 1.0f));
    }
    if (hasActiveAlpha)
    {
        clip(Diffuse.a - 0.25f);
        Diffuse.rgb = pow(Diffuse.rgb, 2.2);
        if (dot(normal, vsin.fragViewPos) >= 0.0f)
        {
            normal = -normal;
        }
    }
    
    float specularColorIntensity = 1.0f;
    float specularPower = 192.f;
    if (hasSpecular)
    {
        specularColorIntensity = specTex.Sample(linearSampling, vsin.texCoord).r;
    }
    
    float3 directLightColor;
    if(vsin.isBillboard)
    {
        directLightColor = globalLightColor * globalLightIntensity * Diffuse.rgb * max(ambientLightIntensity, vsin.lightInstensity);
    }
    else
    {
        directLightColor = CalculateGlobalLightColorSpecular(Diffuse.rgb, normal, vsin.fragViewPos, specularColorIntensity, specularPower);
    }
    
    vsin.fragPosLightSpace.xyz /= vsin.fragPosLightSpace.w;
    directLightColor *= CalculateGlobalLightShadow(vsin.fragPosLightSpace.xyz, shadowMap, shadowMapSampling);
    
    //float3 pointLightColor1 = CalculatePointLightColor(pointLightPos, vsin.fragViewPos, Diffuse.rgb, normal, float3(attConst, attLin, attQuad), specularColorIntensity, specularPowerA, ambient);
    
    //pointLightColor1 *= CalculateOmniLightShadow(vs_in.worldPos, pointLightWorldPos, omniShadowMap, linearSampling, pointLightFarPlane, 0.4f);
    
    //float3 spotlightColor1 = CalculateSpotlightColorWithCookie(spotlightPosition, spotlightDirection.xyz, float3(attConstS, attLinS, attQuadS), vsin.fragViewPos, vsin.fragPosLightSpace, linearSampling, cookieTex, Diffuse.rgb, normal, specularColorIntensity, specularPowerA);
    //float3 spotlightColor1 = CalculateSpotlightColorWithCutoff(spotlightPosition, spotlightDirection, spotlightCutoff, spotlightInnerCutoff, spotlightPower, float3(attConstS, attLinS, attQuadS), vs_in.fragViewPos, Diffuse.rgb, normal, specularColorIntensity, specularPowerA);
    //spotlightColor1.xyz *= CalculateLightShadow(vsin.fragPosLightSpace.xyz, shadowMap, shadowMapSampling, spotlightWidth, spotlightHeight, 0.2f);

    //float3 resultColor = saturate(pointLightColor1 + spotlightColor1 + directLightColor);
    
    //litColor = GenerateDistanceFog(float3(0.1f, 0.1f, 0.1f), 10.f, 500.f, fragViewPos, litColor);
    
    return float4(directLightColor, Diffuse.a);
    
}