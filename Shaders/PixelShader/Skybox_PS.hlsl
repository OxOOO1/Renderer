
Texture2D tex :        register(t0);

SamplerState linearSampling : register(s0);


struct VS_in
{
    float2 texCoord : TexCoord;
    float4 pos : SV_Position;
    
};

float4 main(VS_in vs_in) : SV_Target
{
    //return float4(1.0f, 1.0f, 0.5f, 1.0f);
    //vs_in.texCoord.y = 1 - vs_in.texCoord.y;
   return tex.Sample(linearSampling, vs_in.texCoord);
   
}