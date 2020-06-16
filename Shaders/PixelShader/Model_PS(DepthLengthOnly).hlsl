#include "Functions.hlsl"

cbuffer PointLightCBuf : register(b2)
{
    float3 pointLightPos;
    float specularIntensity;
    float specularPower;
    float attConst;
    float attLin;
    float attQuad;
    float3 pointLightWorldPos;
    float farPlane;
};

struct Gin
{
    float4 posWorld : posWorld;
    float4 Pos : SV_Position;
    uint RTIndex : SV_RenderTargetArrayIndex;
};

float main(Gin gin) : SV_Depth
{
    float lightDistance = length(gin.posWorld.xyz - pointLightWorldPos.xyz);
    
    lightDistance /= farPlane;
    
    return lightDistance;
}
