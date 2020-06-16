
#include "GlobalCBuffer.hlsl"

struct VOut
{
    float4 pos : SV_Position;
    float4 col : Color;
};

VOut main(float3 pos : Position, float4 color : Color) 
{
    VOut vout;
    vout.pos = mul(float4(pos, 1.0f), camViewProjection);
    vout.col = color;
    return vout;
}