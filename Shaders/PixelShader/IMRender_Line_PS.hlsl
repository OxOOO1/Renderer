
struct PSIn
{
    float4 Pos : SV_Position;
    float3 Col : Color;
};

float4 main(in PSIn psin) : SV_Target
{
    return float4(
        psin.Col, 1.0f);
}