
#include "Functions.hlsl"

Texture2D moon : register(t0);

SamplerState samplerLinearClamp;

cbuffer ScatteredLight : register(b2)
{
    float2 posUV;
    float density;
    float decay;
    int numOfSamples;
    float exposure;
    float abberation;
}

float3 main(float2 texCoord : TexCoord) : SV_Target
{
    
        if ((saturate(posUV.x) == posUV.x) && (saturate(posUV.y) == posUV.y))
        {
        
            float weight = length((0.5, 0.5) - posUV) / length((0.5, 0.5));
            weight = clamp(1 - weight, 0.1f, 0.9f);
    
            float2 fromCenterVec = texCoord.xy - float2(0.5, 0.5);
    
            fromCenterVec = clamp(fromCenterVec, (-0.1, -0.1), (0.1, 0.1));
    
            float2 deltaTexCoord = (texCoord - posUV.xy);
            deltaTexCoord *= 1.f / numOfSamples * density;
            
            float3 color;
            
                color = moon.Sample(samplerLinearClamp, texCoord);
            
            float3 rgb;
            float2 vTexCoord = texCoord;
    
            float illuminationDecay = 1.f;
    
            for (int i = 0; i < numOfSamples; i++)
            {
                vTexCoord -= deltaTexCoord;
                
                 rgb = moon.Sample(samplerLinearClamp, vTexCoord);
                   
                
                rgb *= illuminationDecay;
                color += rgb;
        
                illuminationDecay *= decay;
            }
    
            return float3(color * exposure * weight);

        }
    return (0, 0, 0);
    
}
    
    //return float4(result, 1.f);
    
    //float sampleWeight = 1.f;
    
    
    //float2 deltaTexCoord = (texCoord - globalLightPosUV.xy);
    //deltaTexCoord *= 1.f / numOfSamples * density;
    
    //float3 color = tex.Sample(samplerLinearClamp, texCoord);
    
    //float illuminationDecay = 1.f;
    
    //for (int i = 0; i < numOfSamples; i++)
    //{
    //    texCoord -= deltaTexCoord;
        
    //    float3 sample = tex.Sample(samplerLinearClamp, texCoord);
    //    sample *= illuminationDecay * sampleWeight;
    //    color += sample;
        
    //    illuminationDecay *= decay;
    //}
    
    //return float4(color * exposure, 1.f);
    
