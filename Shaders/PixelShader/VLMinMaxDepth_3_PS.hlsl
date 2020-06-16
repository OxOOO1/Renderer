
Texture2D depth : register(t0);

SamplerState samplerPointClamp;

float2 main(float2 texCoord : TexCoord) : SV_Target
{
    float2 depth1 = depth.Sample(samplerPointClamp, texCoord);
    float2 depth2 = depth.Sample(samplerPointClamp, texCoord, int2(0, 1));
    float2 depth3 = depth.Sample(samplerPointClamp, texCoord, int2(1, 1));
    float2 depth4 = depth.Sample(samplerPointClamp, texCoord, int2(1, 0));
    float2 depth5 = depth.Sample(samplerPointClamp, texCoord, int2(1, -1));
    float2 depth6 = depth.Sample(samplerPointClamp, texCoord, int2(0, -1));
    float2 depth7 = depth.Sample(samplerPointClamp, texCoord, int2(-1, -1));
    float2 depth8 = depth.Sample(samplerPointClamp, texCoord, int2(-1, 0));
    float2 depth9 = depth.Sample(samplerPointClamp, texCoord, int2(-1, 1));

    float minDepth = min(depth1.x, depth2.x);
    minDepth = min(minDepth, depth3.x);
    minDepth = min(minDepth, depth4.x);
    minDepth = min(minDepth, depth5.x);
    minDepth = min(minDepth, depth6.x);
    minDepth = min(minDepth, depth7.x);
    minDepth = min(minDepth, depth8.x);
    minDepth = min(minDepth, depth9.x);
	
    float maxDepth = max(depth1.y, depth2.y);
    maxDepth = max(maxDepth, depth3.y);
    maxDepth = max(maxDepth, depth4.y);
    maxDepth = max(maxDepth, depth5.y);
    maxDepth = max(maxDepth, depth6.y);
    maxDepth = max(maxDepth, depth7.y);
    maxDepth = max(maxDepth, depth8.y);
    maxDepth = max(maxDepth, depth9.y);

    return float2(minDepth, maxDepth);
}