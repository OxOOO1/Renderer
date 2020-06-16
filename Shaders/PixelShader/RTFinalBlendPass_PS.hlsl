
Texture2D RT1 : register(t0);
Texture2D RT2 : register(t1);
//Texture2D RT3 : register(t2);

SamplerState samplerState;

static float sampleCount = 2;
static float blur = 0.5;
static float faloff = 2.0;

float4 main(float2 texCoord : TexCoord ) : SV_Target
{
    
    
    return float4(RT1.Sample(samplerState, texCoord).rgb + RT2.Sample(samplerState, texCoord).rgb /*+ RT3.Sample(samplerState, texCoord).rgb*/, 1.0f);
}