
Texture2D depth : register(t0);

SamplerState samplerPointClamp;

float2 main(float2 texCoord : TexCoord ) : SV_Target
{
    float2 textureOffset;
    float width;
    float height;
    depth.GetDimensions(width, height);
    textureOffset.x = 1.f / width / 2.f * 0.25;
    textureOffset.y = 1.f / height / 2.f * 0.25;
	
    float depth1 = depth.Sample(samplerPointClamp, texCoord + float2(textureOffset.x, textureOffset.y));
    float depth2 = depth.Sample(samplerPointClamp, texCoord + float2(textureOffset.x, -textureOffset.y));
    float depth3 = depth.Sample(samplerPointClamp, texCoord + float2(-textureOffset.x, textureOffset.y));
    float depth4 = depth.Sample(samplerPointClamp, texCoord + float2(-textureOffset.x, -textureOffset.y));
		
    float minDepth = min(depth1, depth2);
    minDepth = min(minDepth, depth3);
    minDepth = min(minDepth, depth4);
	
    float maxDepth = max(depth1, depth2);
    maxDepth = max(maxDepth, depth3);
    maxDepth = max(maxDepth, depth4);

    minDepth -= 25.0;
    maxDepth += 25.0;
	
    return float2(minDepth, maxDepth);
}