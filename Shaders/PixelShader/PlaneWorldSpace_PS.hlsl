
cbuffer PlaneInfo : register(b2)
{
    uint numOfInstances;
    uint numOfTextures;
};

struct PS_in
{
    float3 viewPos : Position;
    float3 normal : Normal;
    float3 tan : Tangent;
    float3 bitan : Bitangent;
    float2 tex : TexCoord;
    float4 pos : SV_Position;
    uint instanceID : InstanceID;
};

Texture2DArray textureArray : register(t1);

Texture2D tex : register(t0);

SamplerState samplerState;

float4 main(in PS_in pin) : SV_Target
{
    
    pin.tex.y = 1 - pin.tex.y;
    
    if(numOfTextures == 1)
    {
        return tex.Sample(samplerState, pin.tex);
    }
    else
    {
        float3 uvw = float3(pin.tex, pin.instanceID % numOfTextures);
        return textureArray.Sample(samplerState, uvw);
    }
    
    return float4(1.f, 0.f, 1.f, 1.f);
    
    
}