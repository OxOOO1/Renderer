
#include "GlobalCBuffer.hlsl"

struct VertexIn
{
    float3 position : Position;
    float size : Size;
    uint type : Type;
    float3 color : Color;
    float alphaChannel : AlphaChannel;
};

struct GOut
{
    float4 PosH : SV_POSITION;
    float2 TexCoord : TEXCOORD;
    float3 color : Color;
    float alphaChannel : AlphaChannel;
    //uint PrimID : SV_PrimitiveID;
};

cbuffer GSCbuffer : register(b1)
{
    float3 colorBegin;
    float dTime;
    float3 colorEnd;
    float globalTime;
    
    float particleLifeTime;
};
    
    [maxvertexcount(4)]

void main(point VertexIn gin[1], inout TriangleStream<GOut> triStream)
{
    
    if(gin[0].type == 1)
    {
        float3 viewPos = (float3) mul(float4(gin[0].position, 1.0f), camView);
    
        GOut gout;
        float3 v[4];
        v[0] = viewPos + gin[0].size * float3(1.0f, 0.0f, 0.0f) - gin[0].size * float3(0.0f, 1.0f, 0.0f);
        v[1] = viewPos + gin[0].size * float3(1.0f, 0.0f, 0.0f) + gin[0].size * float3(0.0f, 1.0f, 0.0f);
        v[2] = viewPos - gin[0].size * float3(1.0f, 0.0f, 0.0f) - gin[0].size * float3(0.0f, 1.0f, 0.0f);
        v[3] = viewPos - gin[0].size * float3(1.0f, 0.0f, 0.0f) + gin[0].size * float3(0.0f, 1.0f, 0.0f);
    
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
            gout.alphaChannel = gin[0].alphaChannel;
            gout.color = gin[0].color;
            triStream.Append(gout);
        }
    }
    
    

     
        
}