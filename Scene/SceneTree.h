#pragma once
#include "Components/BoundingVolumes.h"
#include "System/Utilities/DebugDraw.h"
#include "Camera.h"

class SceneTree
{
public:

	static constexpr auto DefaultTreeCapacity = 250;

	SceneTree(const DirectX::XMFLOAT3& center, const DirectX::XMFLOAT3& extents);

	static SceneTree& GetCurrent()
	{
		return *pTree;
	}

	unsigned short InsertBox(AxisAlignedBoundingBox&& newBox);

	void OnUpdate(Camera& mainCamera);

	class Node
	{
	public:

		Node(const DirectX::XMFLOAT3& center, const DirectX::XMFLOAT3& extents)
		{
			nodeAABB.Init(center, extents);
		}

		void VisibilityCheck(Camera& mainCamera);

		void InsertBox(const AxisAlignedBoundingBox& box);

		bool CreateChildren();

		AxisAlignedBoundingBox nodeAABB;
		std::array<unsigned short, 4> indicesToChildren{ 0,0,0,0 };
		std::vector<unsigned short> indicesToModelsAABBoxes;
	};

	std::vector<Node> allTreeNodes;
	std::vector<AxisAlignedBoundingBox> allModelsAABBoxes;

protected:
	static SceneTree* pTree;
	static constexpr float minExtents = 1000.f;
};