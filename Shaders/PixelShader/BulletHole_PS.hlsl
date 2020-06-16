#include "Functions.hlsl"


cbuffer LightCBuf : register(b0)
{
    float3 lightPos;
    float specularIntensity;
    float specularPower;
    float attConst;
    float attLin;
    float attQuad;
};

cbuffer MVP : register(b1)
{
    matrix modelView;
    matrix MVP;
    matrix model;
};

cbuffer DirectLightCBuf : register(b4)
{
    float4 dirLightDirection;
    float3 dirLightColor;
    float dirLightIntensity;
}

cbuffer SpotlightCBuf : register(b5)
{
    float4 spotlightDirection;
    float3 spotlightPosition;
    float spotlightCutoff;
    float spotlightInnerCutoff;
    float spotlightPower;
    float attConstS;
    float attLinS;
    float attQuadS;
}

Texture2D tex : register(t0);

SamplerState samplerState;


float4 main(float3 fragViewPos : Position, float3 n : Normal, float3 tan : Tangent, float3 bitan : Bitangent, float2 texCoord : TexCoord/*, float4 posMain : ProjectorPos*/) : SV_Target
{
    
    float4 ambient = 0.2f.rrrr;
    n = normalize(n);
    float4 Texture = float4(1.0f, 0.0f, 1.f, 1.f);
    float3 normTex = n;
    texCoord.y = 1 - texCoord.y;

    Texture = tex.Sample(samplerState, texCoord);
    
    //if (hasNormal)
    //{
    //    normTex = CalculateNormal(tan, bitan, n, texCoord, normalTex, samplerState);
    //}
    
        clip(Texture.a < 0.1f ? -1 : 1);
        
        //if (dot(normTex, fragViewPos) >= 0.0f)
        //{
        //    normTex = -normTex;
        //}
    
    float specularColorIntensity = 0;
    float specularPowerA = 32.f;
    
    float3 directLightColor = dirLightColor * dirLightIntensity * Texture.rgb * max(0.2f, dot(normalize(dirLightDirection), normTex));
    
    //float3 pointLightColor1 = CalculatePointLightColor(lightPos, fragViewPos, Texture.rgb, normTex, float3(attConst, attLin, attQuad), specularColorIntensity, specularPowerA, ambient);
    
    //float3 spotlightColor1 = CalculateSpotlightColor(spotlightPosition, spotlightDirection.xyz, spotlightCutoff, spotlightPower, float3(attConstS, attLinS, attQuadS), fragViewPos, Texture.rgb, normTex, specularColorIntensity, specularPowerA);

    //float3 litColor = saturate(pointLightColor1 + spotlightColor1 + directLightColor);

    //litColor = GenerateDistanceFog(float3(0.1f, 0.1f, 0.1f), 10.f, 500.f, fragViewPos, litColor);
    
    return float4(directLightColor, Texture.a);
    //return float4(1.0f, 1.0f, 1.f, 1.f);
}