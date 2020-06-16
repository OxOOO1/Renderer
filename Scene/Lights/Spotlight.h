#pragma once
#include "src/System/Rendering.h"

#include "System/RenderTarget.h"
#include "Scene/PointOfView.h"
#include "Components/Drawable/Model.h"
#include "Components/Drawable/Plane.h"

class Spotlights
{
public:

	static constexpr int MaxNumberOfLights = 24;

	Spotlights()
	{
		ReadFromFile();
	}

	~Spotlights()
	{
		WriteToFile();
	}

	void ShowControlWindow(Rendering& renderer);
	int selectedIndex;

	void Draw(Rendering& renderer);

	void OnUpdate(Rendering& renderer);

private:

	void AddNew(Rendering& renderer);

	void WriteToFile();

	void ReadFromFile();


private:

	class Spotlight
	{

	public:
		Spotlight(Rendering& renderer);

		void ShowControlWindow(Rendering& renderer);
		void Draw(Rendering& renderer);

		void OnUpdate(Rendering& renderer);

		UINT id;
		bool bAlignedToCamPos = false;
		DirectX::XMVECTOR xmPosition;
		DirectX::XMVECTOR xmDirection;

		std::shared_ptr<Texture> cookieTexture;

		Billboard lightBillboard;

	};

	std::vector<Spotlight> allSpotlights;

};



/////////////////////Shadow Map//////////////////
//public:
//
//	void BindForShadowMap(Rendering& renderer);
//
//	void SetAsPixelShaderResourceView(Rendering& renderer, UINT slot)
//	{
//		shadowMap.SetAsPixelShaderResourceView(renderer, slot);
//	}
//
//	struct VSCBuffer
//	{
//		DirectX::XMMATRIX spotLightVP;
//	} vsCBufferData;
//
//	VertexConstBuffer<VSCBuffer> vsCBuffer;
//
//	DirectX::XMMATRIX matProjection;
//	DirectX::XMMATRIX matView;
//
//	ShadowMap shadowMap;
//
//	std::shared_ptr<PixelShader> pixelShaderShadowMap;
//	std::shared_ptr<VertexShader> vertexShaderShadowMap;
//
//	bool alignToCamPos = true;