
//cbuffer CBuf : register(b0)
//{
//    matrix modelView;
//    matrix MVP;
//    matrix model;
//    matrix projection;
//};

cbuffer LightCBuf : register(b2)
{
    matrix lightVP[6];
}


struct GIn
{
    float4 Pos : SV_Position;
};

struct GOut
{
    float4 posWorld : posWorld;
    float4 Pos : SV_Position;
    uint RTIndex : SV_RenderTargetArrayIndex;
};
    
    [maxvertexcount(18)]

void main(triangle GIn input[3], inout TriangleStream<GOut> CubeMapStream)
{
    
    for (int f = 0; f < 6; f++)
    {
        
        GOut output;
        
        output.RTIndex = f;
        
        for (int v = 0; v < 3; v++)
        {
            output.posWorld = input[v].Pos;
            
            output.Pos = mul(input[v].Pos, lightVP[f]);
            
            CubeMapStream.Append(output);
        }
        
        CubeMapStream.RestartStrip();

    }
    
    
        
}