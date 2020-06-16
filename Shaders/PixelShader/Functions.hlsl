
#include "GlobalCBuffer.hlsl"

float3 CalculateNormal(const in float3 tan, const in float3 bitan, const in float3 normal, float3 normTex)
{
    normTex.x = normTex.x * 2.0f - 1.0f;
    normTex.y = normTex.y * 2.0f - 1.0f;
    normTex.z = 0.5f;
    normTex = normalize(normTex);
    
    const float3x3 tanToView = float3x3(
        normalize(tan),
        normalize(bitan),
        normal);
    return normTex = normalize(mul(normTex, tanToView));
}


float3 CalculatePointLightColor(uint lightIndex, const in float3 fragViewPos, const in float3 Texture, const in float3 normTex, const float specularIntensity, const float specularPower)
{
    const float3 vTol = pointLights[lightIndex].positionView - fragViewPos;
    const float distToL = length(vTol);
    if (distToL > 3000.f)
    {
        return 0 * Texture;
    }

    const float3 dirToL = vTol / distToL; //normalize(vTol);
    
	//attenuation
    const float att = 1.0f / (pointLights[lightIndex].attConst + pointLights[lightIndex].attLin * distToL + pointLights[lightIndex].attQuad * (distToL * distToL));
    
	
	//diffuse intensity
    float3 diffuse = pointLights[lightIndex].color.rgb * pointLights[lightIndex].colorIntensity * Texture.rgb * att * max(0.f, dot(dirToL, normTex));

    //Blinn_Phong
    float3 viewDir = normalize(-fragViewPos);
    float3 halfwayDir = normalize(dirToL + viewDir);
    float specular = att * specularIntensity * pow(max(dot(normTex, halfwayDir), 0.f), specularPower);

    return diffuse + specular;
}


float3 CalculateGlobalLightColorSpecular(const in float3 Texture, const in float3 normTex, const in float3 fragViewPos, const float specularIntensity, const in float specularPower)
{
    
	//diffuse intensity
    float3 diffuse = globalLightColor.rgb * globalLightIntensity * Texture.rgb * max(ambientLightIntensity, dot(-globalLightDirectionView.xyz, normTex));

    //Blinn_Phong
    float3 viewDir = normalize(-fragViewPos);
    float3 halfwayDir = normalize(-globalLightDirectionView.xyz + viewDir);
    float specular = specularIntensity * 0.5 * globalLightIntensity * pow(max(dot(normTex, halfwayDir), ambientLightIntensity), specularPower);

    return diffuse + specular;
    
}

float3 CalculateGlobalLightColor(const in float3 Texture, const in float3 normTex, const in float3 fragViewPos)
{
    
	//diffuse intensity
    float3 diffuse = globalLightColor.rgb * globalLightIntensity * Texture.rgb * max(ambientLightIntensity, dot(-globalLightDirectionView.xyz, normTex));

    return diffuse;
    
}



float3 CalculateSpotlightColorWithCutoff(uint lightIndex, const in float3 fragViewPos, const in float3 Texture, const in float3 normTex, const in float specularColorIntensity, const in float specularPower)
{

    const float3 vTol = spotLights[lightIndex].positionView - fragViewPos;
    const float distToL = length(vTol);
    //if (distToL > 5000.f){
    //    return 0.f;
    //}

    const float3 dirToL = vTol / distToL; //normalize(vTol);

    float theta = dot(dirToL, -spotLights[lightIndex].directionView.xyz);

    
        //attenuation
    const float att = 1.0f / (spotLights[lightIndex].attConst + spotLights[lightIndex].attLin * distToL + spotLights[lightIndex].attQuad * (distToL * distToL));
	
	//diffuse intensity
    float3 diffuse = Texture * att * max(0.05f, dot(dirToL, normTex));

    //Blinn_Phong
    float3 viewDir = normalize(-fragViewPos);
    float3 halfwayDir = normalize(dirToL + viewDir);
    float specular = att * specularColorIntensity * pow(max(dot(normTex, halfwayDir), 0.05f), specularPower);

    //float spotFactor = pow(theta, spotlightPower);
    float epsilon = spotLights[lightIndex].innerCutoff - spotLights[lightIndex].cutoff;
    float spotFactor = saturate((theta - spotLights[lightIndex].cutoff) / epsilon);

    return (diffuse + specular) * spotFactor * spotLights[lightIndex].power;
   

}

float3 CalculateSpotlightColorWithCookie(const in float3 lightPos, const in float3 spotlightDir, const in float3 attConstLinQuad, const in float3 fragViewPos, const in float3 fragPosLightSpaceNDC, SamplerState linearSampling, Texture2D cookieTex, const in float3 Texture, const in float3 normTex, const in float specularColorIntensity, const in float specularPower)
{

    const float3 vTol = lightPos - fragViewPos;
    const float distToL = length(vTol);
    const float3 dirToL = vTol / distToL;
    
    if (distToL > 5000.f || dot(dirToL, -spotlightDir) < 0.5f)
    {
        return 0.f;
    }

    
    const float att = 1.0f / (attConstLinQuad.x + attConstLinQuad.y * distToL + attConstLinQuad.z * (distToL * distToL));
	
    float3 diffuse = Texture * att * max(0.05f, dot(dirToL, normTex));

    float3 viewDir = normalize(-fragViewPos);
    float3 halfwayDir = normalize(dirToL + viewDir);
    float specular = att * specularColorIntensity * pow(max(dot(normTex, halfwayDir), 0.05f), specularPower);

    float2 projTexCoord;
    float scale = 10.f + distToL / 10000000;
    projTexCoord.x = fragPosLightSpaceNDC.x / 2 + 0.5f;
    projTexCoord.y = -fragPosLightSpaceNDC.y / 2 + 0.5f;
    float cookieModifier = 0.f;
    
    if ((saturate(projTexCoord.x) == projTexCoord.x) && (saturate(projTexCoord.y) == projTexCoord.y))
    {
        cookieModifier = cookieTex.Sample(linearSampling, projTexCoord).r;
    }
    
    return (diffuse + specular) * cookieModifier;
   
    //float theta = max(dot(dirToL, -spotlightDir), 0.0f);
    //float spotFactor = pow(theta, spotlightPower);
    //return (diffuse + specular) * spotFactor;
    

}

float3 GenerateDistanceFog(const in float3 fogColor, const in float fogStartDistance, const in float fogRange, const in float3 fragViewPos, const in float3 Color)
{
    float s = saturate((length(-fragViewPos.xz) - fogStartDistance.rr) / fogRange);
    return (1 - s) * Color + s * fogColor;
}

float LinearizeDepth(float depth, float near, float far)
{
    //depth = depth * 2.f - 1.f;
    return (2.f * near * far) / (far + near - depth * (far - near)) / far;

}

float CalculateGlobalLightShadow(float3 lightSpacePosNDC, Texture2D shadowMap, SamplerComparisonState shadowSampler)
{
    
    if (lightSpacePosNDC.x > 1.f || lightSpacePosNDC.x < -1.f || lightSpacePosNDC.y > 1.f || lightSpacePosNDC.y < -1.f || lightSpacePosNDC.z < 0.f || lightSpacePosNDC.z > 1.f)
    {
        //return ambientLightIntensity;
        return 1.f;
    }
    
    float deltaX = 1.f / shadowMapWidth;
    float deltaY = 1.f / shadowMapHeight;
    
    float2 coords = lightSpacePosNDC.xy * 0.5 + 0.5;
    coords.y = 1 - coords.y;
    
    float result = 0;
    result += shadowMap.SampleCmpLevelZero(shadowSampler, coords, lightSpacePosNDC.z).r;
    
    result += shadowMap.SampleCmpLevelZero(shadowSampler, float2(coords.x - deltaX, coords.y + deltaY), lightSpacePosNDC.z).r;
    result += shadowMap.SampleCmpLevelZero(shadowSampler, float2(coords.x, coords.y + deltaY), lightSpacePosNDC.z).r;
    result += shadowMap.SampleCmpLevelZero(shadowSampler, float2(coords.x + deltaX, coords.y + deltaY), lightSpacePosNDC.z).r;
    
    result += shadowMap.SampleCmpLevelZero(shadowSampler, float2(coords.x - deltaX, coords.y - deltaY), lightSpacePosNDC.z).r;
    result += shadowMap.SampleCmpLevelZero(shadowSampler, float2(coords.x, coords.y - deltaY), lightSpacePosNDC.z).r;
    result += shadowMap.SampleCmpLevelZero(shadowSampler, float2(coords.x + deltaX, coords.y - deltaY), lightSpacePosNDC.z).r;
    
    result += shadowMap.SampleCmpLevelZero(shadowSampler, float2(coords.x - deltaX, coords.y), lightSpacePosNDC.z).r;
    result += shadowMap.SampleCmpLevelZero(shadowSampler, float2(coords.x + deltaX, coords.y), lightSpacePosNDC.z).r;
    
    return max(result /= 9.f, ambientLightIntensity);
    
}

float CalculateOmniLightShadow(float3 fragWorldPos, float3 lightWorldPos, TextureCube shadowMap, SamplerState linearShadowMapSampler, float lightMatrixFarPlane, float ambient)
{
    float3 shadowMapVec = fragWorldPos - lightWorldPos;
    
    float closestDepth = shadowMap.Sample(linearShadowMapSampler, shadowMapVec).r * lightMatrixFarPlane;
    
    return length(shadowMapVec) - 0.9f > closestDepth ? ambient : 1.f;
    
}

float3 GetRandUnitVec(Texture1D tex, SamplerState _sampler, float time, float offset)
{
    
    return tex.SampleLevel(_sampler, time + offset, 0).xyz;
    
    
}
