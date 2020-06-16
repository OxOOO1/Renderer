
#include "SceneTree.h"

SceneTree* SceneTree::pTree = nullptr;

SceneTree::SceneTree(const DirectX::XMFLOAT3 & center, const DirectX::XMFLOAT3 & extents)
{
	SceneTree::pTree = this;

	allTreeNodes.reserve(DefaultTreeCapacity);
	allModelsAABBoxes.reserve(DefaultTreeCapacity);

	allTreeNodes.emplace_back(center, extents);
	allTreeNodes.back().CreateChildren();

}

unsigned short SceneTree::InsertBox(AxisAlignedBoundingBox && newBox)
{
	allModelsAABBoxes.push_back(newBox);

	allTreeNodes.front().InsertBox(allModelsAABBoxes.back());

	return allModelsAABBoxes.size() - 1;
}

void SceneTree::OnUpdate(Camera & mainCamera)
{
	//To avoid storing whole mainCamera object in cache during iterations
	const auto frust = mainCamera.GetFrustrum();
	const auto camPos = mainCamera.GetPositionXM();
	for (auto i : allTreeNodes.front().indicesToModelsAABBoxes)
	{
		allModelsAABBoxes.at(i).bToDraw = mainCamera.CanSee(frust, allModelsAABBoxes.at(i).dAABB);
		allModelsAABBoxes.at(i).distanceToCamera = mainCamera.GetDistance(camPos, allModelsAABBoxes.at(i).dAABB.Center);
	}

	for (auto i : allTreeNodes.front().indicesToChildren)
	{
		allTreeNodes.at(i).VisibilityCheck(mainCamera);
	}
}

void SceneTree::Node::VisibilityCheck(Camera & mainCamera)
{
	//To avoid storing whole mainCamera object in cache during iterations
	const auto frust = mainCamera.GetFrustrum();
	const auto camPos = mainCamera.GetPositionXM();
	if (mainCamera.CanSee(frust, nodeAABB.dAABB))
	{
		float distance = mainCamera.GetDistance(camPos, nodeAABB.dAABB.Center);
		if (distance > 5000)
		{
			for (auto i : indicesToModelsAABBoxes)
			{
				pTree->allModelsAABBoxes.at(i).bToDraw = true;
				pTree->allModelsAABBoxes.at(i).distanceToCamera = distance;
			}
		}
		else
		{
			for (auto i : indicesToModelsAABBoxes)
			{
				pTree->allModelsAABBoxes.at(i).bToDraw = true;
				pTree->allModelsAABBoxes.at(i).distanceToCamera = mainCamera.GetDistance(camPos, pTree->allModelsAABBoxes.at(i).dAABB.Center);
			}
		}

		if (indicesToChildren.at(0) != 0)
		{
			for (auto i : indicesToChildren)
			{
				pTree->allTreeNodes.at(i).VisibilityCheck(mainCamera);
			}
		}

	}
}

void SceneTree::Node::InsertBox(const AxisAlignedBoundingBox & box)
{
	for (auto i : indicesToChildren)
	{
		auto result = pTree->allTreeNodes.at(i).nodeAABB.dAABB.Contains(box.dAABB);
		if (result == AxisAlignedBoundingBox::Contains)
		{
			if (pTree->allTreeNodes.at(i).indicesToChildren.at(0) != 0)
			{
				pTree->allTreeNodes.at(i).InsertBox(box);
				return;
			}
			else if (pTree->allTreeNodes.at(i).CreateChildren())
			{
				pTree->allTreeNodes.at(i).InsertBox(box);
				return;
			}
			else
			{
				indicesToModelsAABBoxes.push_back(pTree->allModelsAABBoxes.size() - 1);
				return;
			}
		}
		else if (result == AxisAlignedBoundingBox::Intersects)
		{
			indicesToModelsAABBoxes.push_back(pTree->allModelsAABBoxes.size() - 1);
			return;
		}
	}

	pTree->allTreeNodes.at(0).indicesToModelsAABBoxes.push_back(pTree->allModelsAABBoxes.size() - 1);

}

bool SceneTree::Node::CreateChildren()
{
	auto& center = nodeAABB.dAABB.Center;
	auto extents = nodeAABB.dAABB.Extents;

	if (fabs(extents.x) <= minExtents || fabs(extents.z) <= minExtents)
	{
		return false;
	}

	std::array<DirectX::XMFLOAT3, 4> centers;
	extents = { extents.x / 2, extents.y, extents.z / 2 };

	centers.at(0) = { center.x + extents.x, center.y, center.z + extents.z };
	centers.at(1) = { center.x + extents.x, center.y, center.z - extents.z };
	centers.at(2) = { center.x - extents.x, center.y, center.z + extents.z };
	centers.at(3) = { center.x - extents.x, center.y, center.z - extents.z };

	for (int i = 0; i < 4; i++)
	{
		indicesToChildren.at(i) = pTree->allTreeNodes.size() + i;
	}

	pTree->allTreeNodes.emplace_back(centers.at(0), extents);
	pTree->allTreeNodes.emplace_back(centers.at(1), extents);
	pTree->allTreeNodes.emplace_back(centers.at(2), extents);
	pTree->allTreeNodes.emplace_back(centers.at(3), extents);

	return true;
}
