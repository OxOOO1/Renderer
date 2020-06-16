

#include "GlobalCBuffer.hlsl"

cbuffer Bone : register(b1)
{
    matrix boneMatrices[200];
}

//cbuffer Node : register(b2)
//{
//    matrix nodeMatrices[200];
//}


struct VS_out
{
    float3 viewPos : Position;
    float3 normal : Normal;
    float3 tan : Tangent;
    float3 bitan : Bitangent;
    float2 tex : TexCoord;
    //float4 projectorPos : ProjectorPos;
    float4 fragPosLightSpace : fragPosLightSpace;
    //float4 worldPos : worldPos;
    float4 pos : SV_Position;
};

VS_out main(float4 pos : Position, float2 tex : TexCoord, uint4 boneIndices : BoneIndices, float4 _weights : Weights, float3 n : Normal, float3 tan : Tangent, float3 bitan : Bitangent)
{
    
    VS_out vso;
    
    //uint nodeMatIndex = _weights.a;
    //uint nodeMatIndex = pos.w;
    
    float weights[4];
    weights[0] = _weights.r;
    weights[1] = _weights.g;
    weights[2] = _weights.b;
    weights[3] = 1.0f - (weights[0] + weights[1] + weights[2]);
    
    vso.pos = float4(0.f, 0.f, 0.f, 0.f);
    vso.fragPosLightSpace = float4(0.f, 0.f, 0.f, 0.f);
    vso.viewPos = float3(0.f, 0.f, 0.f);
    vso.normal = float3(0.f, 0.f, 0.f);
    vso.tan = float3(0.f, 0.f, 0.f);
    vso.bitan = float3(0.f, 0.f, 0.f);
    
    for (int i = 0; i < 4; i++)
    {
        
        if (boneIndices[i] == 0)
            continue;
        
        
        matrix boneSpace = boneMatrices[boneIndices[i]];
        //matrix boneSpace = boneMatrices[nodeMatIndex];
        //boneSpace = mul(boneSpace, boneMatrices[nodeMatIndex]);
        //boneSpace = mul(boneMatrices[nodeMatIndex], boneSpace);
        
        //vso.pos += mul(float4(pos, 1.0f), boneSpace);
        //vso.normal += mul(n, (float3x3) boneSpace);
        //vso.tan += mul(tan, (float3x3) boneSpace);
        //vso.bitan += mul(bitan, (float3x3) boneSpace);
        
        vso.pos += weights[i] * mul(float4(pos.xyz, 1.0f), boneSpace).xyzw;
        
        vso.fragPosLightSpace += weights[i] * mul(float4(pos.xyz, 1.0f), boneSpace);
        matrix modelView = weights[i] * mul(boneSpace, camView);
        vso.viewPos += weights[i] * (float3) mul(float4(pos.xyz, 1.0f), modelView);
        vso.normal += weights[i] * mul(n, (float3x3) modelView);
        vso.tan += weights[i] * mul(tan, (float3x3) modelView);
        vso.bitan += weights[i] * mul(bitan, (float3x3) modelView);
        
    }
     
    //matrix model = boneMatrices[nodeMatIndex];
    //matrix modelView = mul(model, camView);
    //vso.viewPos = mul(vso.pos, modelView);
    //vso.normal = mul(vso.normal, (float3x3) modelView);
    //vso.tan = mul(vso.tan, (float3x3) modelView);
    //vso.bitan = mul(vso.bitan, (float3x3) modelView);
    //vso.pos = mul(float4(vso.pos.xyz, 1.0f), model);
    
    vso.fragPosLightSpace = mul(vso.pos, globalLightVP);
    vso.pos = mul(vso.pos, camViewProjection);
    
    vso.normal = normalize(vso.normal);
    vso.tan = normalize(vso.tan);
    vso.bitan = normalize(vso.bitan);
    
    vso.tex = tex;
    return vso;
}
