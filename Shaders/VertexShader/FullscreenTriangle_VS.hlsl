struct VS_out
{
	float2 tex : TexCoord;
	float4 pos : SV_Position;
};


VS_out main(uint id : SV_VertexID) 
{
	VS_out vso;
	//vso.tex = tex;
 //   //vso.pos = mul(float4(pos, 1.0f), MVP);
 //   vso.pos = float4(pos, 1.0f);
    vso.pos.x = (float) (id / 2) * 4.0f - 1.0f;
    vso.pos.y = (float) (id % 2) * 4.0f - 1.0f;
    vso.pos.z = 0.f;
    vso.pos.w = 1.f;
    
    vso.tex.x = (float) (id / 2) * 2.f;
    vso.tex.y = 1.f - (float) (id % 2) * 2.f;
    
	return vso;
}