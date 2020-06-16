
struct GOut
{
    float4 PosH : SV_POSITION;
    float2 TexCoord : TEXCOORD;
    float3 color : Color;
    float alphaChannel : AlphaChannel;
    //uint PrimID : SV_PrimitiveID;
};

Texture2D tex : register(t1);

SamplerState samplerState;


float4 main(in GOut pin) : SV_Target
{
    
    //pin.TexCoord.y = 1 - pin.TexCoord.y;

    ////float3 uvw = float3(pin.TexCoord, pin.PrimID % 4);
    ////float4 Texture = textureArray.Sample(samplerState, uvw);
    
    float4 Texture = tex.Sample(samplerState, pin.TexCoord);
    
    //clip(pin.alphaChannel - 0.1f);
    
    Texture *= pin.alphaChannel;
    return float4(pin.color, Texture.a);
    
}