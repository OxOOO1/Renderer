struct VS_out
{
    float3 vToFarPlane : vToFarPlane;
	float2 tex : TexCoord;
	float4 pos : SV_Position;
};

cbuffer cbPerFrame : register(b9)
{
    float4 offsets[14];
    float4 frustrumCorners[4];
    
    //float occlusionRadius = 0.5f;
    //float occlusionFadeStart = 0.2f;
    //float occlusionFadeEnd = 2.f;
    //float surfaceEpsilon = 0.05f;
    
};




VS_out main(uint id : SV_VertexID) 
{
	VS_out vso;
	
    if(id == 0)
    {
        vso.vToFarPlane = frustrumCorners[0].xyz;
        vso.pos.xyz = float3(-1.f, -1.f, 0.f);
        vso.pos.w = 1.0f;
        vso.tex = float2(0.f, 1.f);
    }
    if (id == 1)
    {
        vso.vToFarPlane = frustrumCorners[1].xyz;
        vso.pos.xyz = float3(-1.f, 1.f, 0.f);
        vso.pos.w = 1.0f;
        vso.tex = float2(0.f, 0.f);
    }
    if (id == 2)
    {
        vso.vToFarPlane = frustrumCorners[3].xyz;
        vso.pos.xyz = float3(1.f, -1.f, 0.f);
        vso.pos.w = 1.0f;
        vso.tex = float2(1.f, 1.f);
    }
    if (id == 3)
    {
        vso.vToFarPlane = frustrumCorners[2].xyz;
        vso.pos.xyz = float3(1.f, 1.f, 0.f);
        vso.pos.w = 1.0f;
        vso.tex = float2(1.f, 0.f);
    }
    if (id == 4)
    {
        vso.vToFarPlane = frustrumCorners[3].xyz;
        vso.pos.xyz = float3(1.f, -1.f, 0.f);
        vso.pos.w = 1.0f;
        vso.tex = float2(1.f, 1.f);
    }
    if (id == 5)
    {
        vso.vToFarPlane = frustrumCorners[1].xyz;
        vso.pos.xyz = float3(-1.f, 1.f, 0.f);
        vso.pos.w = 1.0f;
        vso.tex = float2(0.f, 0.f);
    }
    
	return vso;
}