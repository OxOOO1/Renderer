#pragma once

#include "Bindable/BindablesH.h"
#include "Components/BoundingVolumes.h"

#include "Material.h"

class Mesh
{
public:

	Mesh() = default;
	Mesh(Rendering& renderer, UINT firstIndexIndex, UINT firstVertexIndex, UINT numOfIndices);

	void Create(Rendering& renderer, UINT firstIndexIndex, UINT firstVertexIndex, UINT numOfIndices);

	void Draw(Rendering& renderer) noexcept;
	void DrawInstanced(Rendering& renderer, unsigned short numOfInstances);

	UINT indexToMaterial = 0;
	UINT indexToBBox = 0;
	UINT indexToNode = 0;

	float cullDistance = 1000.f;

	UINT firstIndexPos = 0;
	UINT firstVertexPos = 0;
	UINT numofIndices = 0;
	UINT numOfVertices = 0;
	
};

static_assert(sizeof(Mesh) < 64);
