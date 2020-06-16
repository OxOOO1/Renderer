#pragma once
#include "src/System/Rendering.h"

#include "src/thirdParty/nlohmann/json.hpp"

#include "System/RenderTarget.h"
#include "Scene/PointOfView.h"
#include "Components/Drawable/Model.h"
#include "Components/Drawable/Plane.h"

#include "System/Utilities/FileIO.h"


class PointLights
{
public:
	static constexpr int MaxNumberOfLights = 24;

	PointLights()
	{
		ReadFromFile();
	}

	~PointLights()
	{
		WriteToFile();
	}

	void ShowControlWindow(Rendering& renderer);
	int selectedIndex;

	void AddNew(Rendering& renderer);

	void OnUpdate(Rendering& renderer);
	void Draw(Rendering& renderer);

	/*DirectX::XMFLOAT3 color{ 1.f,1.f,1.f };
	float colorIntensity = 1.f;

	float attConst = 1.0f;
	float attLin = 0.045f;
	float attQuad = 0.0075f;*/

	void WriteToFile();

	void ReadFromFile();

private:

	class PointLight
	{

		friend class PointLights;

	public:
		PointLight(Rendering& renderer);

		void ShowControlWindow(Rendering& renderer);
		void OnUpdate(Rendering& renderer);
		void Draw(Rendering& renderer);

		void Reset() noexcept;

	protected:

		UINT id;
		bool bAlignedToCamPos = false;
		DirectX::XMVECTOR xmPosition;

		Billboard lightBillboard;


	};


	std::vector<PointLight> allPointLights;

};





//////////////////Omni Shadow Map //////////////////////////
//public:
//
//	void BindForShadowMap(Rendering& renderer)
//	{
//		shadowMap.Bind(renderer);
//
//		auto pos = DirectX::XMLoadFloat3(&transform.position);
//
//		for (int i = 0; i < 6; i++)
//		{
//			const auto& view = DirectX::XMMatrixLookAtLH(
//				pos,
//				DirectX::XMVectorAdd(pos, DirectX::XMLoadFloat3(&targets[i])),
//				DirectX::g_XMIdentityR1
//			);
//
//			gsCBufferData.matView[i] = DirectX::XMMatrixTranspose(
//				DirectX::XMMatrixMultiply(view, matProjection)
//			);
//		}
//
//		gsCBuffer.Update(renderer, gsCBufferData);
//		gsCBuffer.Bind(renderer);
//
//		vsShadowMap->Bind(renderer);
//		gsShadowMap->Bind(renderer);
//		psShadowMap->Bind(renderer);
//
//		cBufferData.lightPosWorld = transform.position;
//
//		pixelcbuf.Update(renderer, cBufferData);
//		pixelcbuf.Bind(renderer);
//
//	}
//
//	void SetAsPixelShaderResourceView(Rendering& renderer, UINT slot)
//	{
//		shadowMap.SetAsPixelShaderResourceView(renderer, slot);
//	}
//
//	DirectX::XMFLOAT3 targets[6] =
//	{
//		{1.0f, 0.f,0.f},
//		{-1.f,0.f,0.f},
//		{0.f,1.f,0.f},
//		{0.f,-1.f,0.f},
//		{0.f,0.f,1.f},
//		{0.f,0.f,-1.f}
//	};
//
//	struct GSCBuffer
//	{
//		DirectX::XMMATRIX matView[6];
//	} gsCBufferData;
//
//	GeometryConstBuffer<GSCBuffer> gsCBuffer;
//
//	DirectX::XMMATRIX matProjection{ DirectX::XMMatrixPerspectiveFovLH(DirectX::XMConvertToRadians(90.f), 1.f, 0.1f, 100000.f) };
//
//	PointLightShadowMap shadowMap;
//
//	std::shared_ptr<VertexShader> vsShadowMap;
//	std::shared_ptr<GeometryShader> gsShadowMap;
//	std::shared_ptr<PixelShader> psShadowMap;
