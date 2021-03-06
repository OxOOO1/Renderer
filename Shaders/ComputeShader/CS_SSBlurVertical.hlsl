
//[numthreads(1024, 1024, 64)]
Texture2D<float4> input : register(t0);
RWTexture2D<float4> output : register(u0);

//static const float weights[13] =
//{
//    0.002216, 0.008764, 0.026995, 0.064759, 0.120985, 0.176033, 0.199471, 0.176033, 0.120985, 0.064759, 0.026995, 0.008764, 0.002216
//};

//static const float weights[33] =
//{
//    0.000476818, 0.00088637, 0.00158309, 0.00271659, 0.00447891, 0.00709492, 0.0107982,
// 0.01579, 0.0221842, 0.0299455, 0.0388372, 0.0483941, 0.0579383, 0.0666449, 0.073654,
// 0.0782085, 0.0797885, 0.0782085, 0.073654, 0.0666449, 0.0579383, 0.0483941,
// 0.0388372, 0.0299455, 0.0221842, 0.01579, 0.0107982, 0.00709492, 0.00447891,
// 0.00271659, 0.00158309, 0.00088637, 0.000476818
//};

static const float weights[65] =
{
    0.000238409, 0.000326682, 0.000443185, 0.000595253, 0.000791545, 0.00104209, 0.0013583,
 0.00175283, 0.00223945, 0.0028327, 0.00354746, 0.00439836, 0.0053991, 0.00656158,
 0.00789502, 0.00940491, 0.0110921, 0.0129518, 0.0149727, 0.0171369, 0.0194186, 0.0217852,
    0.0241971, 0.0266085, 0.0289692, 0.0312254, 0.0333225, 0.0352065,
 0.036827, 0.0381388, 0.0391043, 0.0396953, 0.0398942, 0.0396953, 0.0391043,
    0.0381388, 0.036827, 0.0352065, 0.0333225, 0.0312254, 0.0289692, 0.0266085, 0.0241971,
 0.0217852, 0.0194186, 0.0171369, 0.0149727, 0.0129518, 0.0110921, 0.00940491,
0.00789502, 0.00656158, 0.0053991, 0.00439836, 0.00354746, 0.0028327, 0.00223945,
 0.00175283, 0.0013583, 0.00104209, 0.000791545, 0.000595253, 0.000443185,
 0.000326682, 0.000238409
};

//    static const float weights[129] =
//    {
//        0.000119204, 0.000139713, 0.000163341, 0.000190488, 0.000221592, 0.000257132, 0.000297627,
//0.000343638, 0.000395773, 0.000454678, 0.000521047, 0.000595612, 0.000679148,
// 0.000772467, 0.000876415, 0.000991868, 0.00111973, 0.00126091, 0.00141635,
// 0.00158698, 0.00177373, 0.0019775, 0.00219918, 0.0024396, 0.00269955, 0.00297974,
//0.00328079, 0.00360324, 0.00394751, 0.00431387, 0.00470245, 0.00511325, 0.00554604,
// 0.00600045, 0.00647588, 0.00697153, 0.00748637, 0.00801917, 0.00856843,
// 0.00913245, 0.0097093, 0.0102968, 0.0108926, 0.0114941, 0.0120985, 0.012703, 0.0133043,
// 0.0138992, 0.0144846, 0.0150569, 0.0156127, 0.0161486, 0.0166612, 0.0171472,
//0.0176033, 0.0180263, 0.0184135, 0.018762, 0.0190694, 0.0193334, 0.0195521, 0.019724,
// 0.0198476, 0.0199222, 0.0199471, 0.0199222, 0.0198476, 0.019724, 0.0195521,
//0.0193334, 0.0190694, 0.018762, 0.0184135, 0.0180263, 0.0176033, 0.0171472,
// 0.0166612, 0.0161486, 0.0156127, 0.0150569, 0.0144846, 0.0138992, 0.0133043,
// 0.012703, 0.0120985, 0.0114941, 0.0108926, 0.0102968, 0.0097093, 0.00913245, 0.00856843,
// 0.00801917, 0.00748637, 0.00697153, 0.00647588, 0.00600045, 0.00554604,
// 0.00511325, 0.00470245, 0.00431387, 0.00394751, 0.00360324, 0.00328079, 0.00297974,
// 0.00269955, 0.0024396, 0.00219918, 0.0019775, 0.00177373, 0.00158698, 0.00141635,
// 0.00126091, 0.00111973, 0.000991868, 0.000876415, 0.000772467, 0.000679148,
// 0.000595612, 0.000521047, 0.000454678, 0.000395773, 0.000343638, 0.000297627,
// 0.000257132, 0.000221592, 0.000190488, 0.000163341, 0.000139713, 0.000119204
//    };

bool isLight(float4 color)
{
    return true;
    //return (color.x + color.y + color.z) / 3 > 0.f;
}

#define BLUR_RADIUS 32
#define NUM_OF_THREADS 256
#define CacheSize (NUM_OF_THREADS + 2 * BLUR_RADIUS)
groupshared float4 Cache[CacheSize];

[numthreads(1, NUM_OF_THREADS, 1)]
void main(uint groupIndex : SV_GroupIndex,
    uint3 groupID : SV_GroupID,
    uint3 groupThreadID : SV_GroupThreadID,
    uint3 dispatchThreadID : SV_DispatchThreadID)
{
    
    uint textureLengthX;
    uint textureLengthY;
    input.GetDimensions(textureLengthX, textureLengthY);
    
    if (groupThreadID.y < BLUR_RADIUS) //First thread-group case check
    {
        int y = max(dispatchThreadID.y - BLUR_RADIUS, 0);
        const float4 color = input[int2(dispatchThreadID.x, y)];
        //Cache[groupThreadID.y] = isLight(color) ? color : noLight;
        Cache[groupThreadID.y] = color;
    }

    if (groupThreadID.y >= NUM_OF_THREADS - BLUR_RADIUS) //Last thread group case check
    {
        int y = min(dispatchThreadID.y + BLUR_RADIUS, textureLengthY - 1);
        const float4 color = input[int2(dispatchThreadID.x, y)];
        Cache[groupThreadID.y + 2 * BLUR_RADIUS] = color ;
    }
    
    
    //Main write to buffer
    Cache[groupThreadID.y + BLUR_RADIUS] = input[min(dispatchThreadID.xy, int2(textureLengthX - 1, textureLengthY - 1))];
    //Thread sync
    GroupMemoryBarrierWithGroupSync();
    

    //Blur 
    float4 blurColor = float4(0, 0, 0, 0);

    [unroll]
    for (int i = -BLUR_RADIUS; i <= BLUR_RADIUS; ++i)
    {
        int k = groupThreadID.y + i + BLUR_RADIUS;

        blurColor += weights[i + BLUR_RADIUS] * Cache[k];
    }

    output[dispatchThreadID.xy] = blurColor /*+ input[dispatchThreadID.xy]*/;

    
       

}