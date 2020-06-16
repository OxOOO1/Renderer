#pragma once

#include <queue>
#include "src/Bindable/BindablesH.h"
#include "Shading.h"
#include "src/Scene/SceneTree.h"
#include "Mesh.h"

//TODO RTTR

class Geometry
{
	friend class Importer;
public:

	Geometry(Rendering& renderer, std::string_view mainShaderPath, std::string_view shadowMapShaderPath, std::initializer_list<std::initializer_list<VertexLayout::AttribType>> vertexLayoutsList);

	//Pipeline state setters
	void SetForDepthPass(Rendering& renderer) noexcept;

	template<size_t NumOfVertexBufferStreamsToBind = 2>
	void SetForShadingPass(Rendering & renderer) noexcept
	{
		pVertexShaderShadingPass->Bind(renderer);

		VertexBuffer::BindVertexBuffers<NumOfVertexBufferStreamsToBind>(renderer, mVertexBufferStreams);

		mIndexbuffer.Bind(renderer);
		Topology::Get(renderer, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST).Bind(renderer);

	}

	//Submit visible meshes to draw queue
	void PopulateDrawQueues(SceneTree& octree);

	void DrawShadedFromQueue(Rendering& renderer, Shading& mShading);
	void DrawDepthFromQueue(Rendering& renderer, Shading& mShading);

	void DrawShaded(Rendering& renderer, Shading& mShading);
	void DrawDepth(Rendering& renderer, Shading& mShading);

	void DrawShadedInstanced(Rendering& renderer, Shading& mShading, UINT numOfInstancedToDraw);
	void DrawDepthInstanced(Rendering& renderer, Shading& mShading, UINT numOfInstancesToDraw);

	//Brute force draw all meshes
	void ForceDraw(Rendering& renderer);

protected:
	//RAM Buffers must be filled first!
	void FillGPUBuffersWithRawBuffers(Rendering& renderer)
	{
		assert(mRawVertexBuffers.front().SizeBytes() > 0);
		int size = mVertexBufferStreams.size();
		for (int i = 0; i < size; i++)
		{
			mVertexBufferStreams[i].Create(renderer, mRawVertexBuffers[i]);
		}

		mIndexbuffer.Create(renderer, mRawIndices);
	}

private:

	void CreateVertexBufferStream(std::initializer_list<VertexLayout::AttribType> attribList);

public:

	std::vector<Mesh> mMeshes;

	//For CPU operations
	std::vector<DirectX::XMFLOAT3> mRawVertices;
	std::vector<unsigned short> mRawIndices;

	//Input Assembly
	VertexLayout mVertexLayoutGlobal;
	std::shared_ptr<VertexShader> pVertexShaderShadingPass;
	std::shared_ptr<VertexShader> pVertexShaderDepthPass;

	std::vector<VertexBuffer> mVertexBufferStreams;
	IndexBuffer mIndexbuffer;
	//For Export
	std::vector<RawVertexBuffer> mRawVertexBuffers;

	//MainBoundingBox
	UINT indexToBBox = 0;

	//DrawQueue
	using IndexMaterialPair = std::pair<unsigned short, unsigned short>;
	struct MaterialSort
	{
		bool operator() (const IndexMaterialPair& i1, const IndexMaterialPair& i2)
		{
			return i1.second > i2.second;
		}
	};
	std::priority_queue<IndexMaterialPair, std::vector<IndexMaterialPair>, MaterialSort> indicesToMeshesToDrawMaterialSorted;
	std::priority_queue<IndexMaterialPair, std::vector<IndexMaterialPair>, MaterialSort> indicesToMeshesToDrawMaterialSortedShadowMapPass;

};