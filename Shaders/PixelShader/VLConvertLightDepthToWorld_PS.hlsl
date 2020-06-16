

#include "GlobalCBuffer.hlsl"

Texture2D depth : register(t0);

SamplerState samplerPoint;

//Convert ShadowMap depth to world space
float main(float2 texCoord : TexCoord) : SV_Target
{
    float sceneDepth = depth.Sample(samplerPoint, texCoord.xy);
	
    float4 clipPos;
    clipPos.x = 2.0 * texCoord.x - 1.0;
    clipPos.y = -2.0 * texCoord.y + 1.0;
    clipPos.z = sceneDepth;
    clipPos.w = 1.0;
						
    float4 positionWS = mul(clipPos, globalLightVPInv);
    //normalize
    positionWS.w = 1.0 / positionWS.w;
    positionWS.xyz *= positionWS.w;

    //length
    return dot(positionWS.xyz - globalLightPos, globalLightDirectionWorld.xyz);
}
