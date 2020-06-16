
Texture2D depth : register(t0);

SamplerState samplerPointClamp;

float2 main(float2 texCoord : TexCoord) : SV_Target
{
    float2 textureOffset;
    float width;
    float height;
    depth.GetDimensions(width, height);
    width /= 2.;
    height /= 2.f;
    textureOffset.x = 1.f / width / 2.f * 0.25;
    textureOffset.y = 1.f / height / 2.f * 0.25;
	
    float2 depth1 = depth.Sample(samplerPointClamp, texCoord + float2(textureOffset.x, textureOffset.y));
    float2 depth2 = depth.Sample(samplerPointClamp, texCoord + float2(textureOffset.x, -textureOffset.y));
    float2 depth3 = depth.Sample(samplerPointClamp, texCoord + float2(-textureOffset.x, textureOffset.y));
    float2 depth4 = depth.Sample(samplerPointClamp, texCoord + float2(-textureOffset.x, -textureOffset.y));
	
    float minDepth = min(depth1.x, depth2.x);
    minDepth = min(minDepth, depth3.x);
    minDepth = min(minDepth, depth4.x);

    float maxDepth = max(depth1.y, depth2.y);
    maxDepth = max(maxDepth, depth3.y);
    maxDepth = max(maxDepth, depth4.y);

    return float2(minDepth, maxDepth);
}