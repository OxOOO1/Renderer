#include "Functions.hlsl"


Texture2D tex : register(t0);

SamplerState samplerState;

float4 main(float2 texCoord : TexCoord ) : SV_Target
{
    float4 color = tex.Sample(samplerState, texCoord);
    return color;
}