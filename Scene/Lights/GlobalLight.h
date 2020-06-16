#pragma once
#include "src/System/Rendering.h"



#include "System/RenderTarget.h"
#include "Scene/PointOfView.h"
#include "Components/Drawable/Model.h"
#include "Components/Drawable/Plane.h"
#include "System/GlobalCBuffer.h"


////////////////////////////Global Light//////////////////////////

constexpr int GlobalLightSize = 30000;

namespace
{
	class GlobalLightCascade : public PointOfView
	{
	public:

		GlobalLightCascade() = default;

		GlobalLightCascade(Rendering& renderer, UINT shadowMapSize = 2048, float matrixSize = 2048);

		void ShowControlWindow(Rendering& renderer);

		void OnUpdate(Rendering& renderer, const DirectX::XMVECTOR& direction, const Camera& mainCamera, DirectX::XMVECTOR& frustPos);

		void BindShadowMap(Rendering& renderer);

		void SetShadowMapAsPSResourceView(Rendering& renderer, UINT slot)
		{
			shadowMap.SetAsPixelShaderResourceView(renderer, slot);
		}

		float GetSize()
		{
			return fWidth;
		}

		ShadowMap shadowMap;

		float radius = 0.f;

	};

}

class GlobalLight
{

public:

	GlobalLight(Rendering& renderer, Camera& mainCamera);

	void ShowControlWindow(Rendering& renderer);

	void OnUpdate(Rendering& renderer, Camera& mainCamera);
	
	void DrawMesh(Rendering& renderer)
	{
		mesh.SetPosition(renderer, fPosition);
		mesh.Draw(renderer);
	}

	void BindShadowMap(Rendering& renderer);

	void SetShadowMapAsPSResourceView(Rendering& renderer, UINT slot);

	const DirectX::XMFLOAT3& GetDirection() const noexcept
	{
		return fDirection;
	}
	const DirectX::XMFLOAT3& GetPosition() const noexcept
	{
		return fPosition;
	}
	const DirectX::XMVECTOR& GetDirectionXM() const noexcept
	{
		return xmDirection;
	}

	float GetSize()
	{
		return shadowNear->GetSize();
	}

protected:

	float lightPositionOffset = 1.f;
	DirectX::XMVECTOR lightRotation{ DirectX::XMQuaternionIdentity() };
	DirectX::XMFLOAT3 pitchYawRoll{ 0.f, DirectX::XMConvertToRadians(111.f), DirectX::XMConvertToRadians(50.f) };

	DirectX::XMVECTOR xmDirection;
	DirectX::XMVECTOR xmRight;
	DirectX::XMVECTOR xmUp;
	mutable DirectX::XMVECTOR xmPosition;

	DirectX::XMFLOAT3 fPosition = { 0.0f,3.0f,2.f };
	DirectX::XMFLOAT3 fDirection = { fPosition.x, fPosition.y, fPosition.z + 1 };
	DirectX::XMFLOAT4 fOrientation;

private:

	Billboard mesh;

public:

	std::unique_ptr<GlobalLightCascade> shadowNear;
	std::unique_ptr<GlobalLightCascade> shadowMid;
	std::unique_ptr<GlobalLightCascade> shadowFar;

	AxisAlignedBoundingBox box;
	AxisAlignedBoundingBox box2;
	AxisAlignedBoundingBox box3;

};