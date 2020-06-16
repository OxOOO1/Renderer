
#include "GlobalCBuffer.hlsl"

struct VertexIn
{
    float3 CenterW : POSITION;
    float2 SizeW : SIZE;
};

struct GOut
{
    float4 PosH : SV_POSITION;
    //float3 PosV : POSITION;
    //float3 NormalW : NORMAL;
    float2 TexCoord : TEXCOORD;
    //uint PrimID : SV_PrimitiveID;
};
    
    [maxvertexcount(8)]

void main(point VertexIn gin[1], uint primID : SV_PrimitiveID, inout TriangleStream<GOut> triStream)
{
    
    float3 viewPos = (float3) mul(float4(gin[0].CenterW, 1.0f), camView);
    
    GOut gout;
    float3 v[4];
    v[0] = viewPos + gin[0].SizeW.x * float3(1.0f, 0.0f, 0.0f) - gin[0].SizeW.y * float3(0.0f, 1.0f, 0.0f);
    v[1] = viewPos + gin[0].SizeW.x * float3(1.0f, 0.0f, 0.0f) + gin[0].SizeW.y * float3(0.0f, 1.0f, 0.0f);
    v[2] = viewPos - gin[0].SizeW.x * float3(1.0f, 0.0f, 0.0f) - gin[0].SizeW.y * float3(0.0f, 1.0f, 0.0f);
    v[3] = viewPos - gin[0].SizeW.x * float3(1.0f, 0.0f, 0.0f) + gin[0].SizeW.y * float3(0.0f, 1.0f, 0.0f);

    float2 texC[4] =
    {
        float2(0.f, 1.0f),
        float2(0.f, 0.0f),
        float2(1.f, 1.0f),
        float2(1.f, 0.0f)
    };

    [unroll]
    for (int i = 0; i < 4; i++)
    {
        gout.PosH = mul(float4(v[i], 1.0f), camProjection);
        gout.TexCoord = texC[i];
        triStream.Append(gout);
    }

    
        
        
}