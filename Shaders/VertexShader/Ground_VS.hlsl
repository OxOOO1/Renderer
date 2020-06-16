
#include "GlobalCBuffer.hlsl"

struct VS_out
{
    float texWeight : TexWeight;
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
    
    vso.texWeight = min(1.f, pos.y / 200.f);
    
    vso.tex = tex.xy * 100.;
    vso.pos = mul(float4(pos, 1.0f), camViewProjection);
    vso.fragPosLightSpace = mul(float4(pos, 1.0f), globalLightVP);
    vso.viewPos = (float3) mul(float4(pos, 1.0f), camView);
    vso.normal = mul(n, (float3x3) camView);
    vso.tan = mul(tan, (float3x3) camView);
    vso.bitan = mul(bitan, (float3x3) camView);
    
    return vso;
}
