#include "Functions.hlsl"


cbuffer DescCBuffer : register(b2)
{
    bool hasDiffuse;
    bool hasNormal;
    bool hasSpecular;
    bool hasActiveAlpha;
    bool isEmmiter;
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
Texture2D emitTex : register(t4);
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
    //float4 pos : SV_Position;
};

float4 main(VS_in vsin) : SV_Target
{
    
    
    float3 emitter = emitTex.Sample(linearSampling, vsin.texCoord).rgb;
    if(emitter.r > 0.f)
    {
        float4 Diffuse = tex.Sample(linearSampling, vsin.texCoord);
        if (emitter.g > 0.f)
        {
            return float4(Diffuse.rgb + emitter.rgb , Diffuse.a);
        }
        else
        {
            return float4(Diffuse.rgb + emitter.rrr , Diffuse.a);
        }
        
    }
       
    float4 Diffuse = float4(0.5f, 0.5f, 0.5f, 1.f);
    
    float3 normal = normalize(vsin.n);
    
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
    
    float specularColorIntensity = 1.0f;
    float specularPower = 128.f;
    if (hasSpecular)
    {
        specularColorIntensity = specTex.Sample(linearSampling, vsin.texCoord).r;
    }
    
    //float3 directLightColor = dirLightColor * dirLightIntensity * Diffuse.rgb * max(ambientLightGlobal, dot(normalize(-dirLightDirection.xyz), normal));

    float3 directLightColor = CalculateGlobalLightColorSpecular(Diffuse.rgb, normal, vsin.fragViewPos, specularColorIntensity, specularPower);
    
    vsin.fragPosLightSpace.xyz /= vsin.fragPosLightSpace.w;
    directLightColor *= CalculateGlobalLightShadow(vsin.fragPosLightSpace.xyz, shadowMap, shadowMapSampling);
    
    //float3 pointLightColor1 = CalculatePointLightColor(pointLightPos, vsin.fragViewPos, Diffuse.rgb, normal, float3(attConst, attLin, attQuad), specularColorIntensity, specularPowerA, ambient);
    
    //pointLightColor1 *= CalculateOmniLightShadow(vs_in.worldPos, pointLightWorldPos, omniShadowMap, linearSampling, pointLightFarPlane, 0.4f);
    
    //float3 spotlightColor1 = CalculateSpotlightColorWithCookie(spotlightPosition, spotlightDirection.xyz, float3(attConstS, attLinS, attQuadS), vsin.fragViewPos, vsin.fragPosLightSpace, linearSampling, cookieTex, Diffuse.rgb, normal, specularColorIntensity, specularPowerA);
    //float3 spotlightColor1 = CalculateSpotlightColorWithCutoff(spotlightPosition, spotlightDirection, spotlightCutoff, spotlightInnerCutoff, spotlightPower, float3(attConstS, attLinS, attQuadS), vs_in.fragViewPos, Diffuse.rgb, normal, specularColorIntensity, specularPowerA);
    //spotlightColor1.xyz *= CalculateLightShadow(vsin.fragPosLightSpace.xyz, shadowMap, shadowMapSampling, spotlightWidth, spotlightHeight, 0.2f);

    //float3 resultColor = saturate(pointLightColor1 + spotlightColor1 + directLightColor);
    
    //directLightColor = GenerateDistanceFog(float3(0.1f, 0.1f, 0.1f), 1000.f, 5000.f, vsin.fragViewPos, directLightColor);
    
    return float4(directLightColor, Diffuse.a);
    
}