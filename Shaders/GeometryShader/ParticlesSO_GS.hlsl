

#define EmitterParticle 0
#define FlareParticle 1

#include "GlobalCBuffer.hlsl"

struct Particle
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


    [maxvertexcount(27)]

void main(point Particle vsin[1], inout PointStream<Particle> particleStream)
{
    
    vsin[0].age += dTime;
    
    if (vsin[0].type == 0)
    {
        
        if (vsin[0].age > emitPeriod)
        {
            vsin[0].age = 0.f;
            
            Particle copy = vsin[0];
            if (isDirectionRandom)
            {
                copy.velocity = randTex.SampleLevel(linearSampler, globalTime + vsin[0].age, 0).xyz * spreadRadius;
            }
            if(isPositionRandom)
            {
                copy.position = randTex.SampleLevel(linearSampler, globalTime + vsin[0].age, 0).xyz * spreadRadius;
            }
                
            copy.type = 1;
            particleStream.Append(copy);
            
            for (int i = 0; i < numOfCopies; i++)
            {
                copy.velocity = randTex.SampleLevel(linearSampler, globalTime / i, 0).xyz * spreadRadius;
                particleStream.Append(copy);
            }
        }
        
        particleStream.Append(vsin[0]);
        
    }
    else
    {
        if (vsin[0].age <= particleLifeTime)
        {
            particleStream.Append(vsin[0]);
        }
    }
    
        
        
}