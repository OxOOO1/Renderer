
//[numthreads(1024, 1024, 64)]
Texture2D<float4> input : register(t0);
RWTexture2D<float4> output : register(u0);

static const float weightsWhite[13] =
{
    0.002216, 0.008764, 0.026995, 0.064759, 0.120985, 0.176033, 0.199471, 0.176033, 0.120985, 0.064759, 0.026995, 0.008764, 0.002216
};

static const float weightsBlue[81] =
{
    0.00269955, 0.00297974,
0.00328079, 0.00360324, 0.00394751, 0.00431387, 0.00470245, 0.00511325, 0.00554604,
 0.00600045, 0.00647588, 0.00697153, 0.00748637, 0.00801917, 0.00856843,
 0.00913245, 0.0097093, 0.0102968, 0.0108926, 0.0114941, 0.0120985, 0.012703, 0.0133043,
 0.0138992, 0.0144846, 0.0150569, 0.0156127, 0.0161486, 0.0166612, 0.0171472,
0.0176033, 0.0180263, 0.0184135, 0.018762, 0.0190694, 0.0193334, 0.0195521, 0.019724,
 0.0198476, 0.0199222, 0.0199471, 0.0199222, 0.0198476, 0.019724, 0.0195521,
0.0193334, 0.0190694, 0.018762, 0.0184135, 0.0180263, 0.0176033, 0.0171472,
 0.0166612, 0.0161486, 0.0156127, 0.0150569, 0.0144846, 0.0138992, 0.0133043,
 0.012703, 0.0120985, 0.0114941, 0.0108926, 0.0102968, 0.0097093, 0.00913245, 0.00856843,
 0.00801917, 0.00748637, 0.00697153, 0.00647588, 0.00600045, 0.00554604,
 0.00511325, 0.00470245, 0.00431387, 0.00394751, 0.00360324, 0.00328079, 0.00297974,
 0.00269955
};

static const float weightsGreen[105] =
{
    0.000679148,
 0.000772467, 0.000876415, 0.000991868, 0.00111973, 0.00126091, 0.00141635,
 0.00158698, 0.00177373, 0.0019775, 0.00219918, 0.0024396, 0.00269955, 0.00297974,
0.00328079, 0.00360324, 0.00394751, 0.00431387, 0.00470245, 0.00511325, 0.00554604,
 0.00600045, 0.00647588, 0.00697153, 0.00748637, 0.00801917, 0.00856843,
 0.00913245, 0.0097093, 0.0102968, 0.0108926, 0.0114941, 0.0120985, 0.012703, 0.0133043,
 0.0138992, 0.0144846, 0.0150569, 0.0156127, 0.0161486, 0.0166612, 0.0171472,
0.0176033, 0.0180263, 0.0184135, 0.018762, 0.0190694, 0.0193334, 0.0195521, 0.019724,
 0.0198476, 0.0199222, 0.0199471, 0.0199222, 0.0198476, 0.019724, 0.0195521,
0.0193334, 0.0190694, 0.018762, 0.0184135, 0.0180263, 0.0176033, 0.0171472,
 0.0166612, 0.0161486, 0.0156127, 0.0150569, 0.0144846, 0.0138992, 0.0133043,
 0.012703, 0.0120985, 0.0114941, 0.0108926, 0.0102968, 0.0097093, 0.00913245, 0.00856843,
 0.00801917, 0.00748637, 0.00697153, 0.00647588, 0.00600045, 0.00554604,
 0.00511325, 0.00470245, 0.00431387, 0.00394751, 0.00360324, 0.00328079, 0.00297974,
 0.00269955, 0.0024396, 0.00219918, 0.0019775, 0.00177373, 0.00158698, 0.00141635,
 0.00126091, 0.00111973, 0.000991868, 0.000876415, 0.000772467, 0.000679148

};

static const float weightsRed[129] =
{
    0.000119204, 0.000139713, 0.000163341, 0.000190488, 0.000221592, 0.000257132, 0.000297627,
0.000343638, 0.000395773, 0.000454678, 0.000521047, 0.000595612, 0.000679148,
 0.000772467, 0.000876415, 0.000991868, 0.00111973, 0.00126091, 0.00141635,
 0.00158698, 0.00177373, 0.0019775, 0.00219918, 0.0024396, 0.00269955, 0.00297974,
0.00328079, 0.00360324, 0.00394751, 0.00431387, 0.00470245, 0.00511325, 0.00554604,
 0.00600045, 0.00647588, 0.00697153, 0.00748637, 0.00801917, 0.00856843,
 0.00913245, 0.0097093, 0.0102968, 0.0108926, 0.0114941, 0.0120985, 0.012703, 0.0133043,
 0.0138992, 0.0144846, 0.0150569, 0.0156127, 0.0161486, 0.0166612, 0.0171472,
0.0176033, 0.0180263, 0.0184135, 0.018762, 0.0190694, 0.0193334, 0.0195521, 0.019724,
 0.0198476, 0.0199222, 0.0199471, 0.0199222, 0.0198476, 0.019724, 0.0195521,
0.0193334, 0.0190694, 0.018762, 0.0184135, 0.0180263, 0.0176033, 0.0171472,
 0.0166612, 0.0161486, 0.0156127, 0.0150569, 0.0144846, 0.0138992, 0.0133043,
 0.012703, 0.0120985, 0.0114941, 0.0108926, 0.0102968, 0.0097093, 0.00913245, 0.00856843,
 0.00801917, 0.00748637, 0.00697153, 0.00647588, 0.00600045, 0.00554604,
 0.00511325, 0.00470245, 0.00431387, 0.00394751, 0.00360324, 0.00328079, 0.00297974,
 0.00269955, 0.0024396, 0.00219918, 0.0019775, 0.00177373, 0.00158698, 0.00141635,
 0.00126091, 0.00111973, 0.000991868, 0.000876415, 0.000772467, 0.000679148,
 0.000595612, 0.000521047, 0.000454678, 0.000395773, 0.000343638, 0.000297627,
 0.000257132, 0.000221592, 0.000190488, 0.000163341, 0.000139713, 0.000119204
};

bool isLight(float4 color)
{
    return true;
    //return (color.x + color.y + color.z) / 3 > 0.f;
}

#define WhiteBlurRadius 6
#define BlueBlurRadius 40
#define GreenBlurRadius 52
#define RedBlurRadius 64


#define BLUR_RADIUS 64
#define NUM_OF_THREADS 256
#define CacheSize (NUM_OF_THREADS + 2 * BLUR_RADIUS)
groupshared float4 Cache[CacheSize];

[numthreads(NUM_OF_THREADS, 1, 1)]
void main( uint groupIndex : SV_GroupIndex,
    uint3 groupID : SV_GroupID,
    uint3 groupThreadID : SV_GroupThreadID,
    uint3 dispatchThreadID : SV_DispatchThreadID )
{
    
    
    uint textureLength;
    uint textureLengthY;
    input.GetDimensions(textureLength, textureLengthY);
    
    if (groupThreadID.x < BLUR_RADIUS) //First thread-group case check
    {
        int x = max(dispatchThreadID.x - BLUR_RADIUS, 0);
        const float4 color = input[int2(x, dispatchThreadID.y)];
        Cache[groupThreadID.x] = color;
    }

    if (groupThreadID.x >= NUM_OF_THREADS - BLUR_RADIUS) //Last thread group case check
    {
        int x = min(dispatchThreadID.x + BLUR_RADIUS, textureLength - 1);
        const float4 color = input[int2(x, dispatchThreadID.y)];
        Cache[groupThreadID.x + 2 * BLUR_RADIUS] = color;
    }
    
    
    //Main write to buffer
    Cache[groupThreadID.x + BLUR_RADIUS] = input[min(dispatchThreadID.xy, int2(textureLength - 1, textureLengthY - 1))];
    
    //Thread sync
    GroupMemoryBarrierWithGroupSync();
    
    
    //Blur 
    float4 blurColor = float4(0, 0, 0, 0);

    [unroll]
    for (int i = -RedBlurRadius; i <= RedBlurRadius; ++i)
    {
        int k = groupThreadID.x + i + RedBlurRadius;

        blurColor.r += weightsRed[i + RedBlurRadius] * Cache[k].r;
    }
    
    [unroll]
    for (int i = -GreenBlurRadius; i <= GreenBlurRadius; ++i)
    {
        int k = groupThreadID.x + i + RedBlurRadius;

        blurColor.g += weightsGreen[i + GreenBlurRadius] * Cache[k].g;
    }
    
    [unroll]
    for (int i = -BlueBlurRadius; i <= BlueBlurRadius; ++i)
    {
        int k = groupThreadID.x + i + RedBlurRadius;

        blurColor.b += weightsBlue[i + BlueBlurRadius] * Cache[k].b;
    }
    

    output[dispatchThreadID.xy] = blurColor;



       

}