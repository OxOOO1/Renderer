

struct VS_in
{
    float2 tex : TexCoord;
    float4 pos : SV_Position;
};

Texture2D Diffuse : register(t0);
SamplerState linearSampling : register(s0);

void main(VS_in vsin) 
{
    float alpha = Diffuse.Sample(linearSampling, vsin.tex).a;
    clip(alpha - 0.1f);
}