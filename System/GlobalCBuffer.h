#pragma once

#include "Bindable/ConstantBuffer.h"
#include "src/Scene/Lights/PointLight.h"
#include "src/Scene/Lights/SpotLight.h"

class GlobalCBuffer
{

public:

	static void Bind(Rendering& renderer)
	{
		Get(renderer).vspsgsGlobalCBuffer.Bind(renderer);
	}

	static void Update(Rendering& renderer)
	{
		auto& buffer = Get(renderer);
		buffer.vspsgsGlobalCBuffer.Update(renderer, buffer.data);
		//buffer.vspsgsGlobalCBuffer.Update(renderer, &buffer.data, sizeof(data));
	}

	struct GlobalCBufferData;

	static GlobalCBufferData& GetData(Rendering& renderer)
	{
		return Get(renderer).data;
	}

private:
	GlobalCBuffer(Rendering& renderer)
	{
		vspsgsGlobalCBuffer.Create(renderer, 0, sizeof(data));
	}

	static GlobalCBuffer& Get(Rendering& renderer)
	{
		static GlobalCBuffer gbuffer(renderer);
		return gbuffer;
	}

	struct PointLightCBuf
	{
		DirectX::XMFLOAT3 color{ 1.f,1.f,1.f };
		float colorIntensity = 1.f;

		DirectX::XMFLOAT3 positionViewSpace;
		float specularIntensity = 0.9f;

		float specularPower = 30.f;
		float attConst = 1.0f;
		float attLin = 0.045f;
		float attQuad = 0.0075f;

		DirectX::XMFLOAT3 positionWorldSpace;
		float farPlane{ 100000.f };

	};

	struct SpotlightCBuf
	{
		DirectX::XMFLOAT4 directionViewSpace{ 1.f,1.f,-1.f,0.f };
		DirectX::XMFLOAT3 positionViewSpace{ 0.f,0.f,0.f };
		float cutoff = 0.955f;
		float InnerCutoff = 0.755f;
		float spotlightPower = 1.f;
		float attConst = 1.0f;
		float attLin = 0.045f;
		float attQuad = 0.0001f;
		UINT width = 2000;
		UINT height = 2000;
		float farPlane = 100.f;
	};

	struct GlobalCBufferData
	{
		//Camera

		DirectX::XMMATRIX camView;
		DirectX::XMMATRIX camProjection;
		DirectX::XMMATRIX camViewProjection;

		DirectX::XMMATRIX camProjectionInv;
		DirectX::XMMATRIX camViewProjectionInv;

		DirectX::XMFLOAT3 camPos;
		float pad10;
		DirectX::XMFLOAT3 camRight;
		float pad11;
		DirectX::XMFLOAT3 camUp;
		float pad12;
		DirectX::XMFLOAT3 camDirection;
		float pad13;

		float camNear;
		float camFar;
		float camWidth;
		float camHeight;

		float camWidthInv;
		float camHeightInv;

		UINT pad, pad2;

		//Global Light

		DirectX::XMFLOAT4 globalLightDirectionView;
		DirectX::XMFLOAT3 globalLightColor{ 1.0f,1.f,1.f };
		float globalLightIntensity = 1.0f;

		UINT shadowMapWidth = 2000;
		UINT shadowMapHeight = 2000;
		float ambientLightIntensity = 0.4f;
		float specularLightIntensity = 0.2;

		DirectX::XMMATRIX globalLightVP;
		DirectX::XMMATRIX globalLightVPInv;

		DirectX::XMFLOAT3 globalLightPos;
		float pad6;
		DirectX::XMFLOAT3 globalLightDirectionWorld;
		float pad7;
		DirectX::XMFLOAT3 globalLightRight;
		float pad8;
		DirectX::XMFLOAT3 globalLightUp;
		float pad9;

		//Lights

		UINT numOfPointLights = 0;
		UINT numOfSpotLights = 0;
		UINT pad4;
		UINT pad5;

		std::array<PointLightCBuf, PointLights::MaxNumberOfLights> pointLights;

		std::array<SpotlightCBuf, Spotlights::MaxNumberOfLights> spotLights;


	}data;

	VSPSGSConstBuffer<GlobalCBufferData> vspsgsGlobalCBuffer;
	

};