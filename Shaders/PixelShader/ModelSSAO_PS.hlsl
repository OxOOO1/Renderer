
//cbuffer VP : register(b0)
//{
//    matrix view;
//    matrix projection;
//    matrix VP;
//};

cbuffer ModelCBuffer : register(b1)
{
    matrix model;
};

struct VS_in
{
    float3 normal : Normal;
    float3 viewPos : Position;
    float4 pos : SV_Position;
};

float4 main(VS_in vsin) : SV_Target
{
    
    return float4(normalize(vsin.normal).xyz, vsin.viewPos.z);
    
}