
Texture2D depth : register(t0);

SamplerState samplerPoint;

float main(float2 texCoord : TexCoord) : SV_Target
{
    float depth0 = depth.Sample(samplerPoint, texCoord);
    float threshold = depth0 - 100.0;

    float depth1 = depth.Sample(samplerPoint, texCoord, int2(1, 0));
    float depth2 = depth.Sample(samplerPoint, texCoord, int2(0, -1));
    float depth3 = depth.Sample(samplerPoint, texCoord, int2(-1, 0));
    float depth4 = depth.Sample(samplerPoint, texCoord, int2(0, 1));

    float depthMin = min(depth1, depth2);
    depthMin = min(depthMin, depth3);
    depthMin = min(depthMin, depth4);

    depth1 = depth.Sample(samplerPoint, texCoord, int2(1, 1));
    depth2 = depth.Sample(samplerPoint, texCoord, int2(1, -1));
    depth3 = depth.Sample(samplerPoint, texCoord, int2(-1, -1));
    depth4 = depth.Sample(samplerPoint, texCoord, int2(-1, 1));

    depthMin = min(depthMin, depth1);
    depthMin = min(depthMin, depth2);
    depthMin = min(depthMin, depth3);
    depthMin = min(depthMin, depth4);
	
    return depthMin < threshold ? depthMin : depth0;
}