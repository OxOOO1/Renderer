#pragma once
#include <optional>
#include <string>
#include <vector>
#include <DirectXMath.h>

#include "Components/BoundingVolumes.h"

class Rendering;
class Mesh;
class ModelDynamic;
class SceneTree;

class Node
{
public:
	
	void UpdateOrientationQuat() noexcept
	{
		transform.orientationQuat = DirectX::XMQuaternionRotationRollPitchYaw(transform.rotationEuler.x, transform.rotationEuler.y, transform.rotationEuler.z);
	}
	void UpdatePositionXM()
	{
		transform.positionXM = DirectX::XMLoadFloat3(&transform.position);
	}

	void AddChild(unsigned short indexOfChild) noexcept;

public:
	struct Transform
	{
		DirectX::XMFLOAT3 position{ 0,0,0 };
		DirectX::XMVECTOR positionXM;
		DirectX::XMFLOAT3 rotationEuler{ 0,0,0 };
		DirectX::XMVECTOR orientationQuat{DirectX::XMQuaternionIdentity()};
		float scale = 1;
	} transform;
	
	DirectX::XMMATRIX matLocalTransform{ DirectX::XMMatrixIdentity() };
	DirectX::XMMATRIX matParentSpace{ DirectX::XMMatrixIdentity() };
	DirectX::XMMATRIX matGlobalTransform{ DirectX::XMMatrixIdentity() };
	DirectX::XMMATRIX matInvBindPose{ DirectX::XMMatrixIdentity() };

	UINT index = 0;
	int indexToParent = -1;
	std::vector<unsigned short> indicesToMeshes;
	std::vector<unsigned short> indicesToChildren;
	std::string name;

	bool isAnimated = false;
	bool isJoint = false;

};


/*
	void SetPosition(DirectX::XMFLOAT3 pos);
	void SetOrientation(DirectX::XMFLOAT3 xyz);

	void Translate(DirectX::XMVECTOR& offset) ;
	void Rotate(const DirectX::XMFLOAT3& YPR) ;
	void Scale(float factor) ;
*/
