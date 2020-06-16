
struct GOut
{
    float4 PosH : SV_POSITION;
    //float3 PosV : POSITION;
    //float3 NormalW : NORMAL;
    float2 TexCoord : TEXCOORD;
    //uint PrimID : SV_PrimitiveID;
};

//cbuffer MVP : register(b1)
//{
//    matrix modelView;
//    matrix MVP;
//};

Texture2D tex : register(t0);


SamplerState samplerState;


float4 main(in GOut pin) : SV_Target
{
    
    float2 texCoord = pin.TexCoord;
    texCoord.x = 1. - texCoord.x;
    
    float4 Texture = tex.Sample(samplerState, texCoord);
    
    //clip(Texture.a < 0.1f ? -1 : 1);
    return Texture;
    
    //return float4(1.1,1.1,1.1, Texture.a);
    
}