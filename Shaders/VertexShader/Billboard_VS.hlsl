

struct VS_out
{
    float3 centerW : POSITION;
    float2 sizeW : SIZE;
};


VS_out main(float3 pos : Position, float2 size : Position2D)
{
	VS_out vso;
    if(size.x != 0)
    vso.sizeW = size;
    else
    vso.sizeW = float2(1.0f,1.0f);
    //vso.centerW = mul(pos, (float3x3) modelView);
    //vso.centerW = mul(float4(pos, 1.0f), MVP);
    vso.centerW = pos;
	return vso;
}