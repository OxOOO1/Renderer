#pragma once

//#include "GameObject.h"
#include <array>
#include <cmath>
#include <DirectXCollision.h>

#include "Components/BoundingVolumes.h"
#include "Scene/PointOfView.h"

#include "Bindable/ConstantBuffer.h"
#include "System/Events/Event.h"
#include "System/Events/WindowEvents.h"

class Camera : public PointOfView
{
	friend class Rendering;
	friend class KeyBoardEvents;
	friend class GlobalLight;
public:
	
	Camera(Rendering& renderer, float width, float height, float _near, float _far);
	
	~Camera();

	void OnUpdate(Rendering& renderer);
	void OnEvent(Event& event)
	{
		if(event.CheckTypeAndHandle(EventType::MouseCursorEvent))
		{
			OnMouseCursorEvent(reinterpret_cast<MouseCursorEvent&>(event));
		}
	}

	void OnMouseCursorEvent(MouseCursorEvent& event)
	{
		if (event.state == MouseCursorEvent::CursorState::Hidden)
			isLocked = false;
		else if (event.state == MouseCursorEvent::CursorState::Shown)
			isLocked = true;
	}

	void SpawnControlWindow() noexcept;
	void Reset() noexcept;


	DirectX::XMFLOAT3 ReadPositionFromFile();
	void WritePositionToFile() ;

	bool Intersects(const DirectX::XMFLOAT3& v1, const DirectX::XMFLOAT3& v2, const DirectX::XMFLOAT3& v3, float& distance);
	bool Intersects(const DirectX::BoundingOrientedBox& box, float& distance);
	bool Intersects(const DirectX::BoundingBox& box, float& distance);

private:

	void UpdateDirection();
	void ProcessMouseMovement(float xoffset, float yoffset);
	void ProcessInput();
	void UpdateCBuffer(Rendering& renderer);

	float speedModifier = 1.0f;

	//radians
	float Yaw = 0.f;
	float Pitch = 0.f;

	bool isLocked = false;
	
protected:

	DirectX::XMMATRIX projectionNear;
	DirectX::XMMATRIX projectionMid;
	DirectX::XMMATRIX projectionFar;

	BoundingFrustrum frustrumNear;
	BoundingFrustrum frustrumMid;
	BoundingFrustrum frustrumFar;

};