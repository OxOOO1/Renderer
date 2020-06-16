
#include "GlobalCBuffer.hlsl"

struct VS_out
{
    float3 normal : Normal;
    float3 viewPos : Position;
    float4 pos : SV_Position;
};

//cbuffer Projector : register(b1)
//{
//    matrix projectorVP;
//};

//cbuffer LightCBuf : register(b2)
//{
//    matrix lightVP[6];
//}

VS_out main(float3 pos : Position, float2 tex : TexCoord, float3 n : Normal, float3 tan : Tangent, float3 bitan : Bitangent )
{
    VS_out vso;
    vso.normal = mul(n, (float3x3) camView);
    vso.viewPos = (float3) mul(float4(pos, 1.0f), camView);
    vso.pos = mul(float4(pos, 1.0f), camViewProjection);
    return vso;
}
