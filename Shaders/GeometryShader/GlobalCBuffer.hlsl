
struct PointLight
{
    float3 color;
    float colorIntensity;
    float3 positionView;
    float specularIntensity;
    float specularPower;
    float attConst;
    float attLin;
    float attQuad;
    float3 positionWorld;
    float projectionFarPlane;
};

struct Spotlight
{
    float4 directionView;
    float3 positionView;
    float cutoff;
    float innerCutoff;
    float power;
    float attConst;
    float attLin;
    float attQuad;
    uint width;
    uint height;
};

cbuffer GlobalCBuffer : register(b0)
{
    //Camera

    matrix camView;
    matrix camProjection;
    matrix camViewProjection;

    matrix camProjectionInv;
    matrix camViewProjectionInv;

    float3 camPos;
    float pad10;
    float3 camRight;
    float pad11;
    float3 camUp;
    float pad12;
    float3 camDirection;
    float pad13;

    float camNear;
    float camFar;
    float camWidth;
    float camHeight;
    
    float camWidthInv;
    float camHeightInv;

    uint pad, pad2;

	//Global Light

    float4 globalLightDirectionView;
    float3 globalLightColor;
    float globalLightIntensity = 1.0f;
    
    uint shadowMapWidth = 2000;
    uint shadowMapHeight = 2000;
    float ambientLightIntensity = 0.4f;
    float specularLightIntensity = 0.2;

    matrix globalLightVP;
    matrix globalLightVPInv;

    float3 globalLightPos;
    float pad6;
    float3 globalLightDirectionWorld;
    float pad7;
    float3 globalLightRight;
    float pad8;
    float3 globalLightUp;
    float pad9;


	//PointLight

    uint numOfPointLights = 0;
    uint numOfSpotLights;
    uint pad4;
    uint pad5;
	
    PointLight pointLights[24];

	//SpotLight
    
    Spotlight spotLights[24];

}