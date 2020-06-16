
struct VS_out
{
    float3 newPos : SV_Position;
};

cbuffer CBuf : register(b0)
{
    matrix modelView;
    matrix MVP;
    matrix model;
    matrix projection;
    matrix VP;
    matrix view;
};

float4 main(float3 pos : Position, float3 n : Normal, float3 tan : Tangent, float3 bitan : Bitangent, float2 tex : TexCoord) : SV_Position
{
    
    return mul(float4(pos, 1.0f), model);
    
}
