
Texture2D depth : register(t0);

SamplerState samplerPoint;

float main(float2 texCoord : TexCoord) : SV_Target
{
    float depth0 = depth.Sample(samplerPoint, texCoord);
    float threshold = depth0 + 100.0;

    float depth1 = depth.Sample(samplerPoint, texCoord, int2(1, 0));
    float depth2 = depth.Sample(samplerPoint, texCoord, int2(0, -1));
    float depth3 = depth.Sample(samplerPoint, texCoord, int2(-1, 0));
    float depth4 = depth.Sample(samplerPoint, texCoord, int2(0, 1));

    float depth5 = depth.Sample(samplerPoint, texCoord, int2(1, 1));
    float depth6 = depth.Sample(samplerPoint, texCoord, int2(1, -1));
    float depth7 = depth.Sample(samplerPoint, texCoord, int2(-1, -1));
    float depth8 = depth.Sample(samplerPoint, texCoord, int2(-1, 1));

    float depthMax = max(depth1, depth2);
    depthMax = max(depthMax, depth3);
    depthMax = max(depthMax, depth4);
    depthMax = max(depthMax, depth5);
    depthMax = max(depthMax, depth6);
    depthMax = max(depthMax, depth7);
    depthMax = max(depthMax, depth8);

    float propogate = threshold < depthMax;
    propogate = saturate(propogate);
	
    return lerp(depth0, depthMax, propogate);
}