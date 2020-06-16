#pragma once

#include <DirectXMath.h>
#include "Bindable/ConstantBuffer.h"
#include "Components/BoundingVolumes.h"


class PointOfView
{
	friend class GlobalLight;

public:
	PointOfView(bool Perspective, Rendering& renderer, float width, float height, float _near, float _far)
		: fWidth(width), fHeight(height), fNearPlane(_near), fFarPlane(_far), fRatio(width/height), isPerspective(Perspective)
	{
		UpdateProjectionMatrix();

		frustrum.Init(matrices.projection);

	}

	bool CanSee(DirectX::BoundingBox& box);
	static bool CanSee(const BoundingFrustrum& frustrum, DirectX::BoundingBox& box);
	bool CanSee(DirectX::BoundingOrientedBox& box);
	static bool CanSee(const BoundingFrustrum& frustrum, DirectX::BoundingOrientedBox& box);
	bool CanSee(const DirectX::XMFLOAT3 & v1, const DirectX::XMFLOAT3 & v2, const DirectX::XMFLOAT3 & v3);
	bool CanSee(DirectX::FXMVECTOR& point);

	float GetDistance(const DirectX::XMFLOAT3& point);
	static float GetDistance(const DirectX::XMVECTOR& camPosXM, const DirectX::XMFLOAT3& point);

	static PointOfView& GetMainCamera()
	{
		return *pMainCamera;
	}

	const BoundingFrustrum& GetFrustrum() const noexcept
	{
		return frustrum;
	}

	void SetPosition(const DirectX::XMVECTOR& pos)
	{
		xmPosition = pos;
	}
	void SetDirection(const DirectX::XMVECTOR& dir)
	{
		xmDirection = dir;
		xmRight = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(xmDirection, DirectX::g_XMIdentityR1));
		xmUp = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(xmRight, xmDirection));
	}

	const DirectX::XMFLOAT3& GetPosition() const noexcept
	{
		return fPosition;
	}
	const DirectX::XMFLOAT4& GetOrientation() const noexcept;
	const DirectX::XMVECTOR& GetPositionXM() const noexcept
	{
		return xmPosition;
	}
	const DirectX::XMFLOAT3& GetDirection() const noexcept
	{
		return fDirection;
	}
	const DirectX::XMVECTOR& GetDirectionXM() const noexcept
	{
		return xmDirection;
	}
	const DirectX::XMMATRIX& GetViewMatrix() const noexcept
	{
		return matrices.view;
	}
	DirectX::XMMATRIX GetProjectionMatrix() noexcept
	{
		return matrices.projection;
	}
	const DirectX::XMMATRIX& GetViewProjectionMatrix() const noexcept
	{
		return matrices.viewProjection;
	}
	float GetFarPlaneDistance()
	{
		return fFarPlane;
	}

	std::array<DirectX::XMFLOAT4, 4> GetFrustrumFarCorners();

protected:

	float fFieldOfView{ DirectX::XMConvertToRadians(70.f) };
	float fRatio = 1.f;
	float fNearPlane = 0.1f;
	float fFarPlane = 100.f;
	float fWidth = 100.f;
	float fHeight = 100.f;
	bool isPerspective = true;

	DirectX::XMVECTOR xmDirection;
	DirectX::XMVECTOR xmRight;
	DirectX::XMVECTOR xmUp;
	mutable DirectX::XMVECTOR xmPosition;

	DirectX::XMFLOAT3 fPosition = { 0.0f,3.0f,2.f };
	DirectX::XMFLOAT3 fDirection = { fPosition.x, fPosition.y, fPosition.z + 1 };
	DirectX::XMFLOAT4 fOrientation;
	bool isOrientationLocked = false;

	struct VPMatrices
	{
		DirectX::XMMATRIX view;
		DirectX::XMMATRIX projection;
		DirectX::XMMATRIX viewProjection;
	} matrices;

	void UpdateViewMatrix(Rendering& renderer);
	void UpdateProjectionMatrix();

public:
	BoundingFrustrum frustrum;

	static PointOfView* pMainCamera;
};