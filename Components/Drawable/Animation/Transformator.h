#pragma once

#include "Node.h"

//TODO Stream out transformed vertices

//Index to TransformArray in CBuffer is stored in vertex
class Transform
{
	friend class Importer;
public:

	static constexpr auto CBufferSlot = 1u;

	void Init(Rendering& renderer)
	{
		allNodesFinalTransforms.resize(mNodes.size(), DirectX::XMMatrixIdentity());

		vsMatCBuffer.Create(renderer, CBufferSlot, allNodesFinalTransforms.size() * sizeof(DirectX::XMMATRIX), allNodesFinalTransforms.data());
	}

	void OnUpdate(Rendering& renderer)
	{
		if (bNeedsUpdate)
		{
			UpdateTransforms();
			vsMatCBuffer.Update(renderer, allNodesFinalTransforms.data(), allNodesFinalTransforms.size() * sizeof(DirectX::XMMATRIX));
			bNeedsUpdate = false;
		}
	}

	void Bind(Rendering& renderer)
	{
		vsMatCBuffer.Bind(renderer);
	}

	void ShowUIWindow(std::string_view windowName);

protected:

	void UpdateLocalTransform(UINT nodeIndex);

	void UpdateTransforms(UINT nodeIndex = 0, const DirectX::XMMATRIX& parentTransform = DirectX::XMMatrixIdentity());

	void ExpandNodeTreeUI(UINT indexToNode, std::optional<int>& selectedIndex) const noexcept;

public:
	std::vector<Node> mNodes;
	std::unordered_map<std::string, UINT> mNodeNameToNodeIDMap;

protected:
	std::vector<DirectX::XMMATRIX> allNodesFinalTransforms;
	VertexConstBuffer<DirectX::XMMATRIX> vsMatCBuffer;

	bool bNeedsUpdate = false;
	std::optional<int> selectedNodeGUIIndex;

};