
//Texture2D<float1> tex : register(t0);
Texture2D tex : register(t0);
//Texture2D depth : register(t1);

SamplerState samplerState;


static float blur = 5.5;
static float faloff = 3.0;

float4 main(float2 texCoord : TexCoord, float4 pixelCoord : SV_Position) : SV_Target
{
    
    //float3 color = 0.0f;
    //for (int y = -1; y <= 1; y++)
    //{
    //    for (int x = -1; x <= 1; x++)
    //    {
    //        color += tex.Sample(samplerState, texCoord, int2(x, y)).rgb;
    //    }
    //}
    //color /= 9;
    
    
    float4 color = tex.Sample(samplerState, texCoord);
    float brightness = dot(color.rgb, float3(0.2126f, 0.7152f, 0.0722));
    if (brightness > 1.f /* || depthV > 0.99f*/)
    {
        return float4(color.rgb, 1.f);
    }
    else
        return float4(0.f, 0.f, 0.f, 0.f);

}


////-----------------------------------------------------------------------------
//// Name: DownScale2x2_Lum
//// Type: Pixel shader                                      
//// Desc: Scale down the source texture from the average of 3x3 blocks and
////       convert to grayscale
////-----------------------------------------------------------------------------
//float4 DownScale2x2_Lum(PSInQuad input) : SV_TARGET
//{
//    float3 vColor;
//    float fAvg = 0.0f;
    
//    for (int y = -1; y < 1; y++)
//    {
//        for (int x = -1; x < 1; x++)
//        {
//            // Compute the sum of color values
//            vColor = s0.Sample(samplerLinearClamp, input.texCoord, int2(x, y)).rgb;
//            fAvg += dot(vColor.rgb, LUMINANCE_VECTOR);
//        }
//    }
    
//    fAvg *= 0.25;

//    return float4(fAvg, fAvg, fAvg, 1.0f);
//}


////-----------------------------------------------------------------------------
//// Name: DownScale3x3
//// Type: Pixel shader                                      
//// Desc: Scale down the source texture from the average of 3x3 blocks
////-----------------------------------------------------------------------------
//float4 DownScale3x3(PSInQuad input) : SV_TARGET
//{
//    float fAvg = 0.0f;
//    float vColor;
    
//    for (int y = -1; y <= 1; y++)
//    {
//        for (int x = -1; x <= 1; x++)
//        {
//            // Compute the sum of color values
//            vColor = s0.Sample(samplerLinearClamp, input.texCoord, int2(x, y)).x;
//            fAvg += vColor.r;
//        }
//    }
    
//    // Divide the sum to complete the average
//    fAvg /= 9;

//    return float4(fAvg, fAvg, fAvg, 1.0f);
//}


////-----------------------------------------------------------------------------
//// Name: DownScale3x3_BrightPass
//// Type: Pixel shader                                      
//// Desc: Scale down the source texture from the average of 3x3 blocks
////-----------------------------------------------------------------------------
//float4 DownScale3x3_BrightPass(PSInQuad input) : SV_TARGET
//{
//    float3 vColor = 0.0f;
//    float4 vLum = s1.Sample(samplerPoint, float2(0, 0));
//    float fLum;
    
//    fLum = vLum.r;
       
//    for (int y = -1; y <= 1; y++)
//    {
//        for (int x = -1; x <= 1; x++)
//        {
//            // Compute the sum of color values
//            float4 vSample = s0.Sample(samplerLinearClamp, input.texCoord, int2(x, y));
//            vColor += vSample.rgb;
//        }
//    }
    
//    // Divide the sum to complete the average
//    vColor /= 9;
 
//    // Bright pass and tone mapping
//    vColor = max(0.0f, vColor - BRIGHT_THRESHOLD);
//    vColor *= MIDDLE_GRAY / (fLum + 0.001f);
//    vColor *= (1.0f + vColor / LUM_WHITE);
//    vColor /= (1.0f + vColor);
    
//    return float4(vColor, 1.0f);
//}
