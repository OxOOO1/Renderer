
cbuffer PlaneInfo : register(b1)
{
    uint numOfInstances;
    uint numOfTextures;
};

struct GOut
{
    float4 PosH : SV_POSITION;
    //float3 PosV : POSITION;
    //float3 NormalW : NORMAL;
    float2 TexCoord : TEXCOORD;
    uint PrimID : SV_PrimitiveID;
};


Texture2DArray textureArray : register(t0);

SamplerState samplerState;


float4 main(in GOut pin) : SV_Target
{
    
    //return float4(1.f, 0.f, 1.0f, 1.0f);

    float3 uvw = float3(pin.TexCoord, pin.PrimID % numOfTextures);
    float4 Texture = textureArray.Sample(samplerState, uvw);
    

    //clip(Texture.a - 0.1f);
    return Texture;

    
    
}