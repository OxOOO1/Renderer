
Texture2D depth : register(t0);

SamplerState samplerPoint;

float main(float2 texCoord : TexCoord) : SV_Target
{
    float depth0 = depth.Sample(samplerPoint, texCoord).y;
    float threshold = depth0 - 100.0;

    float depth1 = depth.Sample(samplerPoint, texCoord, int2(1, 0)).y;
    float depth2 = depth.Sample(samplerPoint, texCoord, int2(0, -1)).y;
    float depth3 = depth.Sample(samplerPoint, texCoord, int2(-1, 0)).y;
    float depth4 = depth.Sample(samplerPoint, texCoord, int2(0, 1)).y;

    float depthMin = min(depth1, depth2);
    depthMin = min(depthMin, depth3);
    depthMin = min(depthMin, depth4);

    depth1 = depth.Sample(samplerPoint, texCoord, int2(1, 1)).y;
    depth2 = depth.Sample(samplerPoint, texCoord, int2(1, -1)).y;
    depth3 = depth.Sample(samplerPoint, texCoord, int2(-1, -1)).y;
    depth4 = depth.Sample(samplerPoint, texCoord, int2(-1, 1)).y;

    depthMin = min(depthMin, depth1);
    depthMin = min(depthMin, depth2);
    depthMin = min(depthMin, depth3);
    depthMin = min(depthMin, depth4);
	
    return depthMin < threshold ? depthMin : depth0;
}