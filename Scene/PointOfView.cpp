#include "PointOfView.h"

bool PointOfView::CanSee(DirectX::BoundingBox & box)
{
	auto result = frustrum.dBF.Contains(box);
	return ( result == DirectX::ContainmentType::INTERSECTS || result == DirectX::ContainmentType::CONTAINS);
}

bool PointOfView::CanSee(const BoundingFrustrum & frustrum, DirectX::BoundingBox & box)
{
	auto result = frustrum.dBF.Contains(box);
	return (result == DirectX::ContainmentType::INTERSECTS || result == DirectX::ContainmentType::CONTAINS);
}

bool PointOfView::CanSee(DirectX::BoundingOrientedBox & box)
{
	auto result = frustrum.dBF.Contains(box);
	return (result == DirectX::ContainmentType::INTERSECTS || result == DirectX::ContainmentType::CONTAINS);
}

bool PointOfView::CanSee(const BoundingFrustrum& frustrum, DirectX::BoundingOrientedBox & box)
{
	auto result = frustrum.dBF.Contains(box);
	return (result == DirectX::ContainmentType::INTERSECTS || result == DirectX::ContainmentType::CONTAINS);
}

bool PointOfView::CanSee(const DirectX::XMFLOAT3& v1, const DirectX::XMFLOAT3& v2, const DirectX::XMFLOAT3& v3)
{
	auto xmv1 = DirectX::XMLoadFloat3(&v1);
	auto xmv2 = DirectX::XMLoadFloat3(&v2);
	auto xmv3 = DirectX::XMLoadFloat3(&v3);
	auto result = frustrum.dBF.Contains(xmv1, xmv2, xmv3);
	return (result == DirectX::ContainmentType::INTERSECTS || result == DirectX::ContainmentType::CONTAINS);
}

bool PointOfView::CanSee(DirectX::FXMVECTOR& point)
{
	auto result = frustrum.dBF.Contains(point);
	return (result == DirectX::ContainmentType::INTERSECTS || result == DirectX::ContainmentType::CONTAINS);
}

float PointOfView::GetDistance(const DirectX::XMFLOAT3& point)
{
	float distance;
	DirectX::XMStoreFloat(&distance, DirectX::XMVector3LengthEst(DirectX::XMVectorSubtract(xmPosition, DirectX::XMLoadFloat3(&point))));
	return distance;
}

float PointOfView::GetDistance(const DirectX::XMVECTOR& camPosXM, const DirectX::XMFLOAT3 & point)
{
	float distance;
	DirectX::XMStoreFloat(&distance, DirectX::XMVector3LengthEst(DirectX::XMVectorSubtract(camPosXM, DirectX::XMLoadFloat3(&point))));
	return distance;
}

const DirectX::XMFLOAT4& PointOfView::GetOrientation() const noexcept
{
	return fOrientation;
}

std::array<DirectX::XMFLOAT4, 4> PointOfView::GetFrustrumFarCorners()
{
	std::array<DirectX::XMFLOAT4, 4> t;

	float halfHeight = fFarPlane * tanf(0.5f * fFieldOfView);
	float halfWidth = fRatio * halfHeight;

	t[0] = { -halfWidth, -halfHeight, fFarPlane, 0.f };
	t[1] = { -halfWidth, halfHeight, fFarPlane, 0.f };
	t[2] = { halfWidth, halfHeight, fFarPlane, 0.f };
	t[3] = { halfWidth, -halfHeight, fFarPlane, 0.f };

	return t;
}

void PointOfView::UpdateViewMatrix(Rendering & renderer)
{
	DirectX::XMStoreFloat3(&fDirection, xmDirection);
	DirectX::XMStoreFloat3(&fPosition, xmPosition);

	xmRight = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(xmDirection, DirectX::g_XMIdentityR1));
	xmUp = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(xmRight, xmDirection));

	if (!isOrientationLocked)
	{
		matrices.view = DirectX::XMMatrixLookAtLH(xmPosition, DirectX::XMVectorAdd(xmPosition, xmDirection), xmUp);
		matrices.viewProjection = DirectX::XMMatrixMultiply(matrices.view, matrices.projection);
	}
	

	frustrum.SetPosition(fPosition);
	frustrum.SetOrientation(fOrientation);

}

void PointOfView::UpdateProjectionMatrix()
{
	if (isPerspective)
		matrices.projection = DirectX::XMMatrixPerspectiveFovLH(fFieldOfView, fRatio, fNearPlane, fFarPlane);
	else
		matrices.projection = DirectX::XMMatrixOrthographicLH(fWidth, fHeight, fNearPlane, fFarPlane);

	frustrum.Init(matrices.projection);

}
