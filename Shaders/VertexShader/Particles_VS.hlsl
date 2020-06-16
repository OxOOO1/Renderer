
#include "GlobalCBuffer.hlsl"

struct VS_out
{
    float3 position : Position;
    float size : Size;
    uint type : Type;
    float3 color : Color;
    float alphaChannel : AlphaChannel;
};

struct VS_In
{
    float3 position : Position;
    float3 velocity : Velocity;
    float age : Age;
    uint type : Type;
};

cbuffer GSCbuffer : register(b1)
{
    float3 color1;
    float dTime;
    
    float3 color2;
    float globalTime;
    
    float3 spreadRadius;
    float particleLifeTime;
    
    float3 acceleration;
    float emitPeriod;
    
    float3 position;
    float size;
    
    uint numOfCopies;
    float timeStepModifier;
    
    bool isPositionRandom;
    bool isDirectionRandom;
};


Texture1D randTex : register(t0);

SamplerState linearSampler : register(s0);

VS_out main(VS_In vsin) 
{
    VS_out vsout;
    
    float t = vsin.age / timeStepModifier;
    
    if (vsin.type != 0)
    {
        if(!isPositionRandom)
        {
            vsout.position = 0.5f * t * t * acceleration + t * vsin.velocity + position;
        }
        else
        {
            vsout.position = 0.5f * t * t * acceleration + vsin.position;
        }
    }
    else
    {
        vsout.position = position;
    }
    
    vsout.alphaChannel = 1.0f - smoothstep(0.f, 1.f, vsin.age / particleLifeTime);
    
    vsout.color = lerp(color1, color2, vsin.age / particleLifeTime);
    
    vsout.size = (1.f - vsout.alphaChannel) * size;
    vsout.type = vsin.type;
    
    return vsout;
    
}