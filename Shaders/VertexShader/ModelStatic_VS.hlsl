

#include "GlobalCBuffer.hlsl"

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
};

VS_out main(float3 pos : Position, float2 tex : TexCoord, float3 n : Normal, float3 tan : Tangent, float3 bitan : Bitangent )
{
    VS_out vso;
    vso.tex = tex;
    vso.pos = mul(float4(pos, 1.0f), camViewProjection);
    //vso.worldPos = mul(float4(pos, 1.0f), model);
    vso.fragPosLightSpace = mul(float4(pos, 1.0f), globalLightVP);
    //vso.projectorPos = mul(float4(pos, 1.0f), model);
    //vso.projectorPos = mul(vso.projectorPos, projectorVP);
    vso.viewPos = (float3) mul(float4(pos, 1.0f), camView);
    vso.normal = mul(n, (float3x3) camView);
    vso.tan = mul(tan, (float3x3) camView);
    vso.bitan = mul(bitan, (float3x3) camView);
    return vso;
}
