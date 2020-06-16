

#include "GlobalCBuffer.hlsl"

SamplerState samplerPoint : register(s0);
SamplerState samplerLinear : register(s1);
SamplerState samplerDepthMinMax :register(s2);//point border 10000 color
SamplerComparisonState samplerPoint_Less : register(s3);//border less 0 color
SamplerComparisonState samplerPoint_Greater : register(s4);//border 10000 color


Texture2D<float1> cameraDepth : register(t0);
Texture2D<float1> shadowMapWorldSpace : register(t1);

Texture2D NoiseTexture : register(t2);

Texture2D shadowMapDownsampledMinMax : register(t3);
Texture2D shadowMapHoles : register(t4);


cbuffer VolumeLight : register(b2)
{
    float coarseDepthTexelSize; 
    float samplingRate;
    bool bUseZOptimization;
    bool bUseAngleOptimization;
    float coarseDepthTexelSizeScale;
    float intensity;
    float maxRayLength;
    int maxSteps;
}

float3 main(float2 texCoord : TexCoord) : SV_Target
{
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Ray, Samples, Steps
	////////////////////////////////////////////////////////////////////////////////////////////////////////////
    
    //Get normalized fragment world space pos from camera depth
    float sceneDepth = cameraDepth.Sample(samplerPoint, texCoord.xy);
    float4 clipPos;
    clipPos.x = 2.0 * texCoord.x - 1.0;
    clipPos.y = -2.0 * texCoord.y + 1.0;
    clipPos.z = sceneDepth;
    clipPos.w = 1.0;	
    float4 positionWS = mul(clipPos, camViewProjectionInv);
    positionWS.w = 1.0 / positionWS.w;
    positionWS.xyz *= positionWS.w;
	
    //Get ray
    float3 rayDir = normalize(positionWS.xyz - camPos.xyz);
    float3 posOnNearPlane = camPos.xyz + rayDir * camNear;
    float rayLength = dot(positionWS.xyz - posOnNearPlane, rayDir);
    rayLength = clamp(rayLength, 0.0, maxRayLength); 
	
    if (bUseAngleOptimization)
    {
        float dotViewLight = dot(rayDir, globalLightDirectionWorld);
        rayDir *= exp(dotViewLight * dotViewLight);
    }
		
    //Calculate steps
    rayDir *= samplingRate * 2.0;
    float stepLength = length(rayDir);
    uint stepsNum = min(rayLength / stepLength, maxSteps);

    float jitter = NoiseTexture.Sample(samplerLinear, texCoord.xy).x;
    float scale = stepLength * 0.0005; // Set base brightness factor
    
    // Calculate long step size
    float2 vecForwardProjection;
    vecForwardProjection.x = dot(globalLightRight, rayDir);
    vecForwardProjection.y = dot(globalLightUp, rayDir);
    float longStepScale = int(coarseDepthTexelSize / length(vecForwardProjection) * coarseDepthTexelSizeScale);
    longStepScale = max(longStepScale, 1);
    
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Get ShadowMap TexCoord and Sample's Depth in Light Space
	////////////////////////////////////////////////////////////////////////////////////////////////////////////
    
    float4 shadowUV;
    float3 lightTexCoordAndSampleDepth;
	
    float3 samplePos = posOnNearPlane.xyz + rayDir * jitter;
    shadowUV = mul(float4(samplePos, 1.0), globalLightVP);
    lightTexCoordAndSampleDepth = shadowUV.xyz / shadowUV.w;
    lightTexCoordAndSampleDepth.x = (lightTexCoordAndSampleDepth.x + 1.0) * 0.5;
    lightTexCoordAndSampleDepth.y = (1.0 - lightTexCoordAndSampleDepth.y) * 0.5;
    //Depth
    lightTexCoordAndSampleDepth.z = dot(samplePos - globalLightPos, globalLightDirectionWorld);

    samplePos = posOnNearPlane.xyz + rayDir * (1.0 + jitter);
    shadowUV = mul(float4(samplePos, 1.0), globalLightVP);
    float3 texCoordDepthSample2 = shadowUV.xyz / shadowUV.w;
    texCoordDepthSample2.x = (texCoordDepthSample2.x + 1.0) * 0.5;
    texCoordDepthSample2.y = (1.0 - texCoordDepthSample2.y) * 0.5;
    texCoordDepthSample2.z = dot(samplePos - globalLightPos, globalLightDirectionWorld);

    
    float3 samplesDelta = texCoordDepthSample2 - lightTexCoordAndSampleDepth;
    
    
    float isSampleInLight;
    float2 sampleMinMax;
    float light = 0.0;
    float coordinateZ_end;
    float isLongStep;
    float longStepScale_1 = longStepScale - 1;
    
    //float longStepsNum = 0;
    //float realStepsNum = 0;
	
	[loop]
    for (uint i = 0; i < stepsNum; i++)
    {
        
		////////////////////////////////////////////////////////////////////////////////////////////////////////////
	    // Main sample Comparison
	    ////////////////////////////////////////////////////////////////////////////////////////////////////////////
        
        isSampleInLight = shadowMapWorldSpace.SampleCmpLevelZero(samplerPoint_Less, lightTexCoordAndSampleDepth.xy, lightTexCoordAndSampleDepth.z);
        
		////////////////////////////////////////////////////////////////////////////////////////////////////////////
	    // Attenuation
	    ////////////////////////////////////////////////////////////////////////////////////////////////////////////
        
        sampleMinMax = shadowMapDownsampledMinMax.SampleLevel(samplerDepthMinMax, lightTexCoordAndSampleDepth.xy, 0).xy;
        
        //float propagatedDepth = shadowMapHoles.SampleLevel(samplerPoint, lightTexCoordAndSampleDepth.xy, 0);
        //float attenuation = (lightTexCoordAndSampleDepth.z - propagatedDepth) / ((sampleMinMax.y + attenuationFade) - propagatedDepth);
        //attenuation = saturate(attenuation);
        //attenuation = 1.0 - attenuation;
        //attenuation *= attenuation;
        //float attenuation2 = ((propagatedDepth + attenuationFade) - lightTexCoordAndSampleDepth.z) * (1.0 / attenuationFade);
        //attenuation2 = 1.0 - saturate(attenuation2);
        //attenuation *= attenuation2;
		
        //float isSampleInLightHoles = shadowMapHoles.SampleCmpLevelZero(samplerPoint_Greater, lightTexCoordAndSampleDepth.xy, lightTexCoordAndSampleDepth.z);
        //isSampleInLightHoles *= 10.0 * attenuation;
        //isSampleInLightHoles += 0.25;
        //isSampleInLight *= isSampleInLightHoles;
		
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////
	    // Optimization 
	    ////////////////////////////////////////////////////////////////////////////////////////////////////////////
        
        if (bUseZOptimization)
        {
            coordinateZ_end = lightTexCoordAndSampleDepth.z + samplesDelta.z * longStepScale;
		  
            float comparisonValue = max(lightTexCoordAndSampleDepth.z, coordinateZ_end);
            float isLight = comparisonValue < sampleMinMax.x; // .x stores min depth values
		  
            comparisonValue = min(lightTexCoordAndSampleDepth.z, coordinateZ_end);
            float isShadow = comparisonValue > sampleMinMax.y; // .y stores max depth values
		  
		      // We can perform coarse step if all samples are in light or shadow
            isLongStep = isLight + isShadow;

              //longStepsNum += isLongStep;
              //realStepsNum += 1.0;

            light += scale * isSampleInLight * (1.0 + isLongStep * longStepScale_1); // longStepScale should be >= 1 if we use a coarse step

            lightTexCoordAndSampleDepth += samplesDelta * (1.0 + isLongStep * longStepScale_1);
            i += isLongStep * longStepScale_1;
        }
        else
        {
            light += scale * isSampleInLight;
            lightTexCoordAndSampleDepth += samplesDelta;
        }
        
           //light += scale * isSampleInLight;
           //lightTexCoordAndSampleDepth += samplesDelta;
        
    }

	// Do correction for final coarse steps.
    if (bUseZOptimization)
    {
        light -= scale * isSampleInLight * (i - stepsNum);
    }

	//return longStepsNum / realStepsNum;
	//return light * cos( light );
    return light * intensity * globalLightColor;
}
