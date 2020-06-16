

struct VertexIn
{
    float3 newPos : Position;
    float3 normal : Normal;
    float3 tan : Tangent;
    float3 bitan : Bitangent;
    float2 tex : TexCoord;
};

//struct GOut
//{
//    float3 newPos : Position;
//    float3 normal : Normal;
//    float3 tan : Tangent;
//    float3 bitan : Bitangent;
//    float2 tex : TexCoord;
//};
    
    [maxvertexcount(1)]

void main(point VertexIn gin[1], inout PointStream<VertexIn> vertexStream)
{
    
        
    vertexStream.Append(gin[0]);
    

    
        
        
}