#pragma once

#include <DirectXCollision.h>
#include <vector>

#include "Bindable/ConstantBuffer.h"
#include "Bindable/PixelShader.h"



class OrientedBoundingBox
{
	
	friend class Model;
	friend class ModelSkinned;
	friend class Node;
	friend class Mesh;
	
public:

	void Draw(Rendering& renderer)  noexcept;
	void Init(Rendering& renderer, UINT numOfVertices, DirectX::XMFLOAT3* vertices, UINT sizeOfVertex);
	void Init(Rendering& renderer, const DirectX::XMFLOAT3& center, const DirectX::XMFLOAT3& extents);

	void SetPosition(const DirectX::XMFLOAT3& pos) noexcept;
	void SetOrientation(const DirectX::XMFLOAT4& orientation);

	const DirectX::XMFLOAT3& GetPosition() const noexcept
	{
		return dOBB.Center;
	}

	void Translate(const DirectX::XMVECTOR& translation);
	void Rotate(const DirectX::XMVECTOR& rotation);
	void Scale(float scale);

	bool IsInView(Rendering& renderer);

	void SpawnControlWindow();

	DirectX::BoundingOrientedBox dOBB;
	float size = 0;
protected:

	bool bToDraw = false;
	float distanceToCamera = 0.f;
};

static_assert(sizeof(OrientedBoundingBox) <= 64);

class AxisAlignedBoundingBox
{
	friend class Geometry;
	friend class SceneTree;
	friend class Instancer;
	friend class ModelLOD;
public:
	void Draw()  noexcept;
	void Init(UINT numOfVertices, DirectX::XMFLOAT3* vertices, UINT sizeOfVertex);
	void Init(const DirectX::XMFLOAT3& center, const DirectX::XMFLOAT3& extents);

	void SetPosition(const DirectX::XMFLOAT3& pos) noexcept;
	
	DirectX::XMFLOAT3 GetPosition() const noexcept
	{
		return dAABB.Center;
	}
	
	void Translate(const DirectX::XMVECTOR& translation);
	void Scale(float scale);

	bool IsInView(Rendering& renderer);

	static constexpr auto Contains = DirectX::ContainmentType::CONTAINS;
	static constexpr auto Intersects = DirectX::ContainmentType::INTERSECTS;

	void GetCorners(DirectX::XMFLOAT3* corners)
	{
		dAABB.GetCorners(corners);
	}

	void SpawnControlWindow();

	DirectX::BoundingBox dAABB;
	float size = 0;
	float distanceToCamera = 0.f;
protected:
	bool bToDraw = false;

};

static_assert(sizeof(AxisAlignedBoundingBox) <= 64);


class BoundingFrustrum
{
	friend class PointOfView;
	friend class Camera;
	friend class GlobalLight;
public:

	void Draw(Rendering& renderer)  noexcept;
	void Init(const DirectX::XMMATRIX& projection);

	void Update(DirectX::XMMATRIX projection, const DirectX::XMFLOAT3& pos, const DirectX::XMFLOAT4& orientation)
	{
		dBF = DirectX::BoundingFrustum(projection);

		//dBF.GetCorners(corners.data());

		SetPosition(pos);
		SetOrientation(orientation);
	}

	void Update(DirectX::XMMATRIX projection, const DirectX::XMVECTOR& pos, const DirectX::XMVECTOR& orientation)
	{
		dBF = DirectX::BoundingFrustum(projection);

		//dBF.GetCorners(corners.data());
		
		SetPosition(pos);
		SetOrientation(orientation);
	}

	std::array<DirectX::XMFLOAT3, 8> GetCorners()
	{
		std::array<DirectX::XMFLOAT3, 8> corners;
		dBF.GetCorners(corners.data());
		return corners;
	}

	void SetPosition(const DirectX::XMFLOAT3& pos) noexcept;
	void SetOrientation(const DirectX::XMFLOAT4& orientation);
	void SetPosition(const DirectX::XMVECTOR& pos) noexcept;
	void SetOrientation(const DirectX::XMVECTOR& orientation);

	void UpdateScale(float scale) noexcept;

	void SpawnControlWindow();

public:
	DirectX::BoundingFrustum dBF;

protected:

	DirectX::XMFLOAT3 center;
	//std::array<DirectX::XMFLOAT3, 8> corners;
	
private:

	bool isViewLocked = false;
	bool isPosLocked = false;
};

static_assert(sizeof(BoundingFrustrum) < 128);