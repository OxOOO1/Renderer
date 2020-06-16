#include "Geometry.h"

Geometry::Geometry(Rendering & renderer, std::string_view mainShaderPath, std::string_view shadowMapShaderPath, std::initializer_list<std::initializer_list<VertexLayout::AttribType>> vertexLayoutsList)
{

	for (auto layout : vertexLayoutsList)
	{
		CreateVertexBufferStream(layout);
	}

	pVertexShaderShadingPass = VertexShader::Resolve(renderer, mainShaderPath, mVertexLayoutGlobal);
	if(shadowMapShaderPath != "")
	pVertexShaderDepthPass = VertexShader::Resolve(renderer, shadowMapShaderPath, mRawVertexBuffers.front().GetVertexLayout());

}

void Geometry::SetForDepthPass(Rendering & renderer) noexcept
{
	pVertexShaderDepthPass->Bind(renderer);
	//mVertexBufferStreams.front().Bind(renderer);
	VertexBuffer::BindVertexBuffers<1>(renderer, mVertexBufferStreams);
	mIndexbuffer.Bind(renderer);
	Topology::Get(renderer, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST).Bind(renderer);

}

void Geometry::PopulateDrawQueues(SceneTree & octree)
{
	static constexpr auto ShadowDistanceMod = 1.f / 4.f;

	if (octree.allModelsAABBoxes[indexToBBox].bToDraw)
	{
		auto size = mMeshes.size();
		for (int i = 0; i < size; i++)
		{
			auto& mesh = mMeshes[i];
			auto& bbox = octree.allModelsAABBoxes[mesh.indexToBBox];

			if (bbox.bToDraw || bbox.distanceToCamera < mesh.cullDistance * ShadowDistanceMod)
			{
				bbox.Draw();
				indicesToMeshesToDrawMaterialSortedShadowMapPass.push(std::make_pair(i, mesh.indexToMaterial));
			}

			if (bbox.bToDraw && bbox.distanceToCamera < mesh.cullDistance)
			{
				indicesToMeshesToDrawMaterialSorted.push(std::make_pair(i, mesh.indexToMaterial));
				bbox.bToDraw = false;
			}
		}

		octree.allModelsAABBoxes[indexToBBox].bToDraw = false;

	}
}

void Geometry::ForceDraw(Rendering & renderer)
{
	for (auto& m : mMeshes)
	{
		m.Draw(renderer);
	}
}

void Geometry::DrawShadedFromQueue(Rendering & renderer, Shading & mShading)
{
	if (!indicesToMeshesToDrawMaterialSorted.empty())
	{
		SetForShadingPass(renderer);
		mShading.Bind(renderer);

		short activeMaterialIndex = -1;
		while (!indicesToMeshesToDrawMaterialSorted.empty())
		{
			auto& mesh = mMeshes.at(indicesToMeshesToDrawMaterialSorted.top().first);
			if (mesh.indexToMaterial != activeMaterialIndex)
			{
				activeMaterialIndex = mesh.indexToMaterial;
				mShading.mMaterials.at(mesh.indexToMaterial).Bind(renderer);
			}
			mesh.Draw(renderer);
			indicesToMeshesToDrawMaterialSorted.pop();
		}
	}
}

void Geometry::DrawDepthFromQueue(Rendering & renderer, Shading & mShading)
{
	if (!indicesToMeshesToDrawMaterialSortedShadowMapPass.empty())
	{
		SetForDepthPass(renderer);

		short activeMaterialIndex = -1;
		while (!indicesToMeshesToDrawMaterialSortedShadowMapPass.empty())
		{
			assert(indicesToMeshesToDrawMaterialSortedShadowMapPass.top().first < mMeshes.size());
			auto& mesh = mMeshes.at(indicesToMeshesToDrawMaterialSortedShadowMapPass.top().first);
			if (mesh.indexToMaterial != activeMaterialIndex)
			{
				activeMaterialIndex = mesh.indexToMaterial;
				assert(activeMaterialIndex < mShading.mMaterials.size());
				mShading.mMaterials[activeMaterialIndex].BindForDepthPass(renderer);
			}
			mesh.Draw(renderer);
			indicesToMeshesToDrawMaterialSortedShadowMapPass.pop();
		}
	}
}

void Geometry::DrawShaded(Rendering & renderer, Shading & mShading)
{
	
	SetForShadingPass(renderer);
	mShading.Bind(renderer);

	short activeMaterialIndex = -1;
	for(auto& mesh : mMeshes)
	{
		if (mesh.indexToMaterial != activeMaterialIndex)
		{
			activeMaterialIndex = mesh.indexToMaterial;
			mShading.mMaterials.at(mesh.indexToMaterial).Bind(renderer);
		}
		mesh.Draw(renderer);
	}
	
}

void Geometry::DrawDepth(Rendering & renderer, Shading & mShading)
{
	SetForDepthPass(renderer);

	short activeMaterialIndex = -1;
	for(auto& mesh : mMeshes)
	{
		if (mesh.indexToMaterial != activeMaterialIndex)
		{
			activeMaterialIndex = mesh.indexToMaterial;
			mShading.mMaterials[activeMaterialIndex].BindForDepthPass(renderer);
		}
		mesh.Draw(renderer);
	}
	
}

void Geometry::DrawShadedInstanced(Rendering & renderer, Shading & mShading, UINT numOfInstancedToDraw)
{
	SetForShadingPass(renderer);
	mShading.Bind(renderer);

	short activeMaterialIndex = -1;
	for (auto& mesh : mMeshes)
	{
		if (mesh.indexToMaterial != activeMaterialIndex)
		{
			activeMaterialIndex = mesh.indexToMaterial;
			mShading.mMaterials.at(mesh.indexToMaterial).Bind(renderer);
		}
		mesh.DrawInstanced(renderer, numOfInstancedToDraw);
	}
}

void Geometry::DrawDepthInstanced(Rendering & renderer, Shading & mShading, UINT numOfInstancedToDraw)
{
	SetForDepthPass(renderer);

	short activeMaterialIndex = -1;
	for (auto& mesh : mMeshes)
	{
		if (mesh.indexToMaterial != activeMaterialIndex)
		{
			activeMaterialIndex = mesh.indexToMaterial;
			mShading.mMaterials[activeMaterialIndex].BindForDepthPass(renderer);
		}
		mesh.DrawInstanced(renderer, numOfInstancedToDraw);
	}
}

void Geometry::CreateVertexBufferStream(std::initializer_list<VertexLayout::AttribType> attribList)
{
	UINT slot = mVertexBufferStreams.size();
	mVertexLayoutGlobal.Append(attribList, slot);
	mVertexBufferStreams.emplace_back();
	VertexLayout tL{ attribList , slot };
	mRawVertexBuffers.emplace_back(tL);
}
