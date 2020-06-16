
#include "Functions.hlsl"

Texture2D tex : register(t0);

SamplerState samplerLinearClamp;

static float sampleCount = 2;
static float blur = 0.5;
static float faloff = 0.5;

float4 main(float2 texCoord : TexCoord) : SV_Target
{
    
    float2 fromCenter = texCoord.xy - float2(0.5, 0.5);
    fromCenter = normalize(fromCenter);
    //fromCenter *= 0.2;
    fromCenter = fromCenter * blur * pow(length(texCoord.xy - (0.5, 0.5)), faloff);
    float R = tex.Sample(samplerLinearClamp, float2(texCoord.x - fromCenter.x / 25, texCoord.y - fromCenter.y / 25));
    float G = tex.Sample(samplerLinearClamp, float2(texCoord.x - fromCenter.x / 50, texCoord.y - fromCenter.y / 50));
    float B = tex.Sample(samplerLinearClamp, float2(texCoord.x + fromCenter.x / 100, texCoord.y + fromCenter.y / 100));
    
    
    return float4(tex.Sample(samplerLinearClamp, texCoord).rgb + float3(R, G, B), 1);
    
    //return float4(R, G, B, 1.f);
    
    //float3 color = float3(R, G, B);
    
    
}


//For Loading Screen
//float2 vTexCoord = -texCoord + float2(1., 0);
    
    //float2 fromCenterVec = float2(0.5, 0.5) - vTexCoord.xy;
    ////fromCenterVec.xy *= 5.f;
    
    //float4 result = 0;
    
    
    //for (int i = 0; i < 3; i++)
    //{
    //    float2 offset = frac(vTexCoord + fromCenterVec * float(i));
        
        
    //    result += tex.Sample(samplerLinearClamp, offset);
    
    //}
    
    //return result;