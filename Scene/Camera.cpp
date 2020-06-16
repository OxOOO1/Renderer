#include "Camera.h"

#include <fstream>
#include <sstream>
#include "System/GlobalCBuffer.h"
#include "thirdParty/ImGUI/imgui.h"
#include "src/thirdParty/nlohmann/json.hpp"
#include "Bindable/BindablesH.h"
#include "System/Rendering.h"
#include "src/System/Utilities/Console.h"
#include "Components/BoundingVolumes.h"
#include "System/Utilities/DebugDraw.h"
#include "System/Input/Keyboard.h"
#include "System/Input/Mouse.h"
#include "System/Utilities/Time.h"
#include "System/Painter.h"

Camera::Camera(Rendering & renderer, float width, float height, float _near, float _far)
	: PointOfView(true, renderer, width, height, _near, _far)
{
	fPosition = ReadPositionFromFile();
	frustrum.SetPosition(fPosition);
	xmPosition = DirectX::XMLoadFloat3(&fPosition);
	UpdateDirection();

	PointOfView::pMainCamera = this;


	projectionNear = DirectX::XMMatrixPerspectiveFovLH(fFieldOfView, fRatio, fNearPlane, _far/ 50.f);
	frustrumNear.Init(projectionNear);
	projectionMid = DirectX::XMMatrixPerspectiveFovLH(fFieldOfView, fRatio, _far / 50.f, _far / 4.f);
	frustrumMid.Init(projectionMid);
	projectionFar = DirectX::XMMatrixPerspectiveFovLH(fFieldOfView, fRatio, _far / 4.f, _far);
	frustrumFar.Init(projectionFar);


}

void Camera::OnUpdate(Rendering& renderer)
{
	if (!isLocked)
	{
		ProcessInput();
		auto offset = Mouse::Get().GetRawOffset();
		ProcessMouseMovement(offset.x, offset.y);
		UpdateViewMatrix(renderer);
		UpdateCBuffer(renderer);
	}

}


void Camera::UpdateCBuffer(Rendering& renderer)
{
	
	
	auto& data = GlobalCBuffer::GetData(renderer);
	
	data.camView = DirectX::XMMatrixTranspose(matrices.view);
	data.camProjection = DirectX::XMMatrixTranspose(matrices.projection);
	data.camViewProjection = DirectX::XMMatrixTranspose(matrices.viewProjection);
	
	data.camViewProjectionInv = DirectX::XMMatrixTranspose(DirectX::XMMatrixInverse(nullptr, matrices.viewProjection));
	data.camProjectionInv = DirectX::XMMatrixTranspose(DirectX::XMMatrixInverse(nullptr, matrices.projection));
	
	data.camNear = fNearPlane;
	data.camFar = fFarPlane;
	
	data.camWidth = fWidth;
	data.camHeight = fHeight;
	data.camWidthInv = 1.f / fWidth;
	data.camHeightInv = 1.f / fHeight;

	DirectX::XMStoreFloat3(&data.camPos, xmPosition);
	DirectX::XMStoreFloat3(&data.camRight, xmRight);
	DirectX::XMStoreFloat3(&data.camUp, xmUp);
	DirectX::XMStoreFloat3(&data.camDirection, xmDirection);

}


Camera::~Camera()
{
	WritePositionToFile();
}

void Camera::SpawnControlWindow() noexcept
{
	if (ImGui::Begin("CameraControl"))
	{
		if(ImGui::Button("Lock"))
		{
			isLocked = !isLocked;
		}
		ImGui::Checkbox("LockOrientation", &isOrientationLocked);

		/*ImGui::SliderFloat3("Position", &fPosition.x, -50.f, 50.f);
		ImGui::SliderFloat3("Direction", &fDirection.x, -50.f, 50.f);*/
		ImGui::SliderFloat("Speed", &speedModifier, 0.1f, 5.f);
		if (ImGui::Button("Reset"))
		{
			Reset();
		}

		/*bool toUpdate = false;

		if (ImGui::SliderFloat("FarPlane", &fFarPlane, 1000.f, 100000.f))
			toUpdate = true;
		if (ImGui::SliderFloat("NearPlane", &fNearPlane, 0.1f, 100.f))
			toUpdate = true;
		ImGui::DragFloat("Width", &fWidth);
		ImGui::DragFloat("Height", &fHeight);

		if (toUpdate)
			UpdateProjectionMatrix();*/
		
		
	}
	ImGui::End();

}

void Camera::Reset() noexcept
{
	xmPosition = { 0.0f, 1.0f, 0.0f};

	DirectX::XMStoreFloat3(&fPosition, xmPosition);
	frustrum.SetPosition(fPosition);
}


 void Camera::ProcessInput()
 {
	 auto dTime = Time::Get().GetDeltaTimeMs();
	 float velocity = Keyboard::IsKeyPressed(VK_SHIFT) ? 3.f : 1.f;
	 if (Keyboard::IsKeyPressed('W'))
		 xmPosition = DirectX::XMVectorAdd(xmPosition, DirectX::XMVectorScale(xmDirection, velocity * speedModifier * dTime));
	 if (Keyboard::IsKeyPressed('S'))
		 xmPosition = DirectX::XMVectorAdd(xmPosition, DirectX::XMVectorNegate(DirectX::XMVectorScale(xmDirection, velocity * speedModifier * dTime)));
	 if (Keyboard::IsKeyPressed('A'))
		 xmPosition = DirectX::XMVectorAdd(xmPosition, DirectX::XMVectorScale(xmRight, velocity * speedModifier * dTime));
	 if (Keyboard::IsKeyPressed('D'))
		 xmPosition = DirectX::XMVectorAdd(xmPosition, DirectX::XMVectorNegate(DirectX::XMVectorScale(xmRight, velocity * speedModifier * dTime)));

	 DirectX::XMStoreFloat3(&fPosition, xmPosition);
	 
 }

 DirectX::XMFLOAT3 Camera::ReadPositionFromFile()
 {
	 std::ifstream ifile("saves.json");
	 nlohmann::json jj = nlohmann::json::parse(ifile);
	 auto pos = jj.find("CameraPosition");
	 std::vector<float> camPos = pos.value();

	 auto dir = jj.find("CameraYawPitch");
	 Yaw = dir.value().at(0);
	 Pitch = dir.value().at(1);
	 
	 auto speed = jj.find("Speed");
	 speedModifier = speed.value();

	 ifile.close();
	 return { camPos.at(0), camPos.at(1), camPos.at(2) };
 }

 void Camera::WritePositionToFile() 
 {
	 std::ofstream ofile("saves.json");
	 nlohmann::json jj; 
	 DirectX::XMFLOAT3 xmPos;
	 //DirectX::XMFLOAT3 xmDir;
	 DirectX::XMStoreFloat3(&xmPos, xmPosition);
	 
	 std::vector<float> vpos = { xmPos.x,xmPos.y, xmPos.z };
	 
	 jj["CameraPosition"] = vpos;
	 jj["CameraYawPitch"] = { Yaw, Pitch };
	 jj["Speed"] = speedModifier;
	 ofile.clear();
	 ofile << jj;
 }

bool Camera::Intersects(const DirectX::XMFLOAT3& v1, const DirectX::XMFLOAT3& v2, const DirectX::XMFLOAT3& v3, float& distance)
{
	return DirectX::TriangleTests::Intersects(
		xmPosition,
		xmDirection,
		DirectX::XMLoadFloat3(&v1),
		DirectX::XMLoadFloat3(&v2),
		DirectX::XMLoadFloat3(&v3),
		distance
	);
}

bool Camera::Intersects(const DirectX::BoundingOrientedBox & box, float& distance)
{
	return (box.Intersects(xmPosition, xmDirection, distance) );
}

bool Camera::Intersects(const DirectX::BoundingBox & box, float & distance)
{
	return (box.Intersects(xmPosition, xmDirection, distance));
}

 void Camera::ProcessMouseMovement(float xoffset, float yoffset)
 {
	 Yaw = DirectX::XMScalarModAngle(Yaw + xoffset);
	 Pitch += yoffset;

	 if (Pitch > 1.2f)
	 {
		 Pitch = 1.2f;
	 }
	 if (Pitch < -1.2f) 
	 {
		 Pitch = -1.2f;
	 }

	 //WORKS
	 auto Yquat = DirectX::XMQuaternionRotationRollPitchYaw(-Pitch, -Yaw - DirectX::XMConvertToRadians(270), 0.0f);
	 DirectX::XMStoreFloat4(&fOrientation, Yquat);

	 if(!isLocked)
	 UpdateDirection();

 }

 void Camera::UpdateDirection()
{
	DirectX::XMFLOAT3 front;
	front.x = DirectX::XMScalarCos(Yaw) * DirectX::XMScalarCos(Pitch);
	front.y = DirectX::XMScalarSin(Pitch);
	front.z = DirectX::XMScalarSin(Yaw) * DirectX::XMScalarCos(Pitch);
	xmDirection = DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&front));

}
