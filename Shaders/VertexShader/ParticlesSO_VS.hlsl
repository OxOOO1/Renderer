
struct VS_out
{
    float3 position : Position;
    float3 velocity : Velocity;
    float age : Age;
    uint type : Type;
};


VS_out main(VS_out vsin) 
{
    return vsin;
}