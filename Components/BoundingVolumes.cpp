#include "BoundingVolumes.h"

#include "Bindable/BindablesH.h"
#include "Scene/Camera.h"
#include "thirdParty/ImGUI/imgui.h"

#include "System/Utilities/DebugDraw.h"


void AxisAlignedBoundingBox::Draw()  noexcept
{
	
	DebugDraw::DrawCube(dAABB);

}

void AxisAlignedBoundingBox::Init(UINT numOfVertices, DirectX::XMFLOAT3 * vertices, UINT sizeOfVertex)
{
	DirectX::BoundingBox::CreateFromPoints(
		dAABB,
		numOfVertices,
		vertices,
		sizeOfVertex
	);
}

void AxisAlignedBoundingBox::Init(const DirectX::XMFLOAT3 & center, const DirectX::XMFLOAT3 & extents)
{
	dAABB = DirectX::BoundingBox(center, extents);

	size = (dAABB.Extents.x + dAABB.Extents.y + dAABB.Extents.z) / 3;
}

void AxisAlignedBoundingBox::SetPosition(const DirectX::XMFLOAT3& position) noexcept
{
	dAABB.Center = position;
}

void AxisAlignedBoundingBox::Translate(const DirectX::XMVECTOR & translation)
{
	dAABB.Transform(dAABB, 0.0f, DirectX::XMQuaternionIdentity(), translation);
}


void AxisAlignedBoundingBox::Scale(float factor)
{
	auto temp = dAABB.Center;
	dAABB.Center = { 0.0f,0.0f,0.0f };
	dAABB.Transform(dAABB, factor, DirectX::XMQuaternionIdentity(), { 0.f,0.f,0.f,0.f });
	dAABB.Center = temp;
}

bool AxisAlignedBoundingBox::IsInView(Rendering & renderer)
{
	return PointOfView::GetMainCamera().CanSee(dAABB);
}

void AxisAlignedBoundingBox::SpawnControlWindow()
{
	/*if (ImGui::Begin("Box"))
	{
		if (ImGui::SliderFloat3("Position", &pos.x, -20.f, 20.f))
		{
			dAABB.Transform(dAABB, DirectX::XMMatrixTranslation(pos.x, pos.y, pos.z));
		}
	}
	ImGui::End();*/
}



//------------------------------------/////OBB/////////////////////////////--------------------------------------------///////////////////////////////

void OrientedBoundingBox::Draw(Rendering & renderer) noexcept
{
	DebugDraw::DrawCube(dOBB, DebugDraw::Color::RANDOM);
}

void OrientedBoundingBox::Init(Rendering & renderer, UINT numOfVertices, DirectX::XMFLOAT3 * vertices, UINT sizeOfVertex)
{
	DirectX::BoundingBox box;

	DirectX::BoundingBox::CreateFromPoints(
		box,
		numOfVertices,
		vertices,
		sizeOfVertex
	);

	DirectX::BoundingOrientedBox::CreateFromBoundingBox(dOBB, box);

	size = (dOBB.Extents.x + dOBB.Extents.y + dOBB.Extents.z) / 3;

}

void OrientedBoundingBox::Init(Rendering & renderer, const DirectX::XMFLOAT3 & center, const DirectX::XMFLOAT3 & extents)
{
	dOBB = DirectX::BoundingOrientedBox(center, extents, { 0.0f,0.0f,0.0f,1.0f });

}

void OrientedBoundingBox::SetPosition(const DirectX::XMFLOAT3 & pos) noexcept
{
	dOBB.Center = pos;
}

void OrientedBoundingBox::SetOrientation(const DirectX::XMFLOAT4 & orientation)
{
	dOBB.Orientation = orientation;
}

void OrientedBoundingBox::Rotate(const DirectX::XMVECTOR & rotation)
{
	auto quat2 = DirectX::XMQuaternionMultiply(rotation, DirectX::XMLoadFloat4(&dOBB.Orientation));
	DirectX::XMStoreFloat4(&dOBB.Orientation, quat2);
}

void OrientedBoundingBox::Translate(const DirectX::XMVECTOR & translation)
{
	dOBB.Transform(dOBB, 0.0f, DirectX::XMQuaternionIdentity(), translation);
}

void OrientedBoundingBox::Scale(float factor)
{
	auto temp = dOBB.Center;
	dOBB.Center = { 0.0f,0.0f,0.0f };
	dOBB.Transform(dOBB, factor, DirectX::XMQuaternionIdentity(), { 0.f,0.f,0.f,0.f });
	dOBB.Center = temp;
}

bool OrientedBoundingBox::IsInView(Rendering & renderer)
{
	return PointOfView::GetMainCamera().CanSee(dOBB);
}

void OrientedBoundingBox::SpawnControlWindow()
{
	/*if (ImGui::Begin("Box"))
	{
		if (ImGui::SliderFloat3("Position", &pos.x, -20.f, 20.f))
		{
			dAABB.Transform(dAABB, DirectX::XMMatrixTranslation(pos.x, pos.y, pos.z));
		}
	}
	ImGui::End();*/

}



void BoundingFrustrum::Draw(Rendering & renderer) noexcept
{

	DebugDraw::DrawFrustrum(dBF, DebugDraw::Color::RED);

}

void BoundingFrustrum::Init(const DirectX::XMMATRIX& projection)
{
	dBF = DirectX::BoundingFrustum(projection);
	//dBF.GetCorners(corners.data());

	auto centerXM = DirectX::XMLoadFloat3(&center);

	auto corners = GetCorners();

	for (int i = 0; i < 8; i++)
	{
		centerXM = DirectX::XMVectorAdd(centerXM, DirectX::XMLoadFloat3(&corners.at(i)));
	}

	DirectX::XMStoreFloat3(&center, DirectX::XMVectorScale(centerXM, 1. / 8.));

}

void BoundingFrustrum::SetPosition(const DirectX::XMFLOAT3 & pos) noexcept
{
	if (isPosLocked)return;
	dBF.Origin = pos;
	//dBF.GetCorners(corners.data());
	auto centerXM = DirectX::XMLoadFloat3(&center);

	auto corners = GetCorners();
	for (int i = 0; i < 8; i++)
	{
		centerXM = DirectX::XMVectorAdd(centerXM, DirectX::XMLoadFloat3(&corners.at(i)));
	}

	DirectX::XMStoreFloat3(&center, DirectX::XMVectorScale(centerXM, 1. / 8.));
}

void BoundingFrustrum::SetPosition(const DirectX::XMVECTOR & pos) noexcept
{
	DirectX::XMFLOAT3 posf;
	DirectX::XMStoreFloat3(&posf, pos);
	SetPosition(posf);
}

void BoundingFrustrum::SetOrientation(const DirectX::XMFLOAT4& orientation)
{
	if (isViewLocked)return;
	dBF.Orientation = orientation;
	//dBF.GetCorners(corners.data());
}
void BoundingFrustrum::SetOrientation(const DirectX::XMVECTOR& orientation)
{
	if (isViewLocked)return;
	DirectX::XMStoreFloat4(&dBF.Orientation, orientation);
	//dBF.GetCorners(corners.data());
}

void BoundingFrustrum::UpdateScale(float scale) noexcept
{
	
	dBF.Transform(dBF, DirectX::XMMatrixScaling(scale, scale, scale));
	//dBF.GetCorners(corners.data());

}

void BoundingFrustrum::SpawnControlWindow()
{
	if (ImGui::Begin("CameraFrustrumControl"))
	{

		if (ImGui::Button("LockView"))
		{
			isViewLocked = !isViewLocked;
		}
		if (ImGui::Button("LockPosition"))
		{
			isPosLocked = !isPosLocked;
		}


		if (ImGui::Button("ScaleDown"))
		{
			UpdateScale(0.1f);
		}
		

	}
	ImGui::End();
}

