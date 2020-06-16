#include "Mesh.h"
#include "src/Bindable/BindablesH.h"


Mesh::Mesh(Rendering & renderer, UINT firstIndexIndex, UINT firstVertexIndex, UINT numOfIndices)
	: firstIndexPos(firstIndexIndex),
	firstVertexPos(firstVertexIndex),
	numofIndices(numOfIndices)
{
}

void Mesh::Create(Rendering& renderer, UINT firstIndexIndex, UINT firstVertexIndex, UINT numOfIndices)
{
	firstIndexPos = firstIndexIndex;
	firstVertexPos = firstVertexIndex;
	this->numofIndices = numOfIndices;
}

void Mesh::Draw(Rendering & renderer) noexcept
{
	renderer.DrawIndexed(numofIndices, firstIndexPos, firstVertexPos);
}

void Mesh::DrawInstanced(Rendering & renderer, unsigned short numOfInstances)
{
	renderer.GetContext()->DrawIndexedInstanced(numofIndices, numOfInstances, firstIndexPos, firstVertexPos, 0u);
}


