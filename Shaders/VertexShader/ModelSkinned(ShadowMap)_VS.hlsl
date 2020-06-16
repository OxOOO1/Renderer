


#include "GlobalCBuffer.hlsl"

cbuffer Model : register(b1)
{
    //max 96 bones per character
    matrix boneMatrices[200];
}


struct VS_out
{
    //float3 viewPos : Position;
    //float3 normal : Normal;
    //float3 tan : Tangent;
    //float3 bitan : Bitangent;
    //float2 tex : TexCoord;
    ////float4 projectorPos : ProjectorPos;
    //float4 fragPosLightSpace : fragPosLightSpace;
    ////float4 worldPos : worldPos;
    float4 pos : SV_Position;
};

//cbuffer Projector : register(b1)
//{
//    matrix projectorVP;
//};

//cbuffer LightCBuf : register(b2)
//{
//    matrix lightVP[6];
//}

VS_out main(float4 pos : Position, float2 tex : TexCoord, uint4 boneIndices : BoneIndices, float4 _weights : Weights)
{
    
    VS_out vso;
    
    float weights[4];
    weights[0] = _weights[0];
    weights[1] = _weights[1];
    weights[2] = _weights[2];
    weights[3] = 1.0f - weights[0] - weights[1] - weights[2];
    
    vso.pos = float4(0.f, 0.f, 0.f, 0.f);
    //vso.fragPosLightSpace = float4(0.f, 0.f, 0.f, 0.f);
    //vso.viewPos = float3(0.f, 0.f, 0.f);
    //vso.normal = float3(0.f, 0.f, 0.f);
    //vso.tan = float3(0.f, 0.f, 0.f);
    //vso.bitan = float3(0.f, 0.f, 0.f);
    
    for (int i = 0; i < 4; i++)
    {
        
        //matrix model = boneMatrices[boneIndices[i]] * weights[i];
        matrix model = boneMatrices[boneIndices[i]] * weights[i];
        
        vso.pos += mul(float4(pos.xyz, 1.0f), model);
        
        //vso.fragPosLightSpace += mul(float4(pos, 1.0f), model);
        
        //matrix modelView = mul(model, camView);
        //vso.viewPos += (float3) mul(float4(pos, 1.0f), modelView);
        //vso.normal += mul(n, (float3x3) modelView);
        //vso.tan += mul(tan, (float3x3) modelView);
        //vso.bitan += mul(bitan, (float3x3) modelView);
        
        //vso.pos += weights[i] * mul(float4(pos, 1.0f), boneMatrices[boneIndices[i]]);
        
        //vso.fragPosLightSpace += weights[i] * mul(float4(pos, 1.0f), boneMatrices[boneIndices[i]]);
        
        //matrix modelView = weights[i] * mul(boneMatrices[boneIndices[i]], view);
        //vso.viewPos += weights[i] * (float3) mul(float4(pos, 1.0f), modelView);
        //vso.normal += weights[i] * mul(n, (float3x3) modelView);
        //vso.tan += weights[i] * mul(tan, (float3x3) modelView);
        //vso.bitan += weights[i] * mul(bitan, (float3x3) modelView);
    }
    
        
    //vso.tex = tex;
    //vso.pos = mul(float4(pos, 1.0f), model);
    vso.pos = mul(vso.pos, globalLightVP);
    //vso.worldPos = mul(float4(pos, 1.0f), model);
    //vso.fragPosLightSpace = mul(float4(pos, 1.0f), model);
    //vso.fragPosLightSpace = mul(vso.fragPosLightSpace, globalLightVP);
    //vso.projectorPos = mul(float4(pos, 1.0f), model);
    //vso.projectorPos = mul(vso.projectorPos, projectorVP);
    
    //matrix modelView = mul(model, view);
    //vso.viewPos = (float3) mul(float4(pos, 1.0f), modelView);
    //vso.normal = mul(n, (float3x3) modelView);
    //vso.tan =    mul(tan, (float3x3) modelView);
    //vso.bitan =  mul(bitan, (float3x3) modelView);
    return vso;
}
