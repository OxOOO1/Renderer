#include "Plane.h"
#include "src/Bindable/BindablesH.h"
#include <d3dcompiler.h>
#include "src/thirdParty/ImGUI/imgui.h"


Plane::Plane(Rendering& renderer)
  : mTopologyType{Topology::Get(renderer, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP)},
	mSamplerState{Sampler::Get(renderer)}
{
	mIndexbuffer.Create(renderer, indices);
}

LocalSpacePlane::LocalSpacePlane(Rendering & renderer, const DirectX::XMVECTOR & position, const DirectX::XMVECTOR & normal, float scale)
	:Plane(renderer)
{
	auto nRight = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(normal, DirectX::g_XMIdentityR1));
	auto nUp = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(nRight, normal));

	for (auto& v : vertices)
	{
		DirectX::XMStoreFloat3(&v.normal, normal);
		DirectX::XMStoreFloat3(&v.tangent, nRight);
		DirectX::XMStoreFloat3(&v.bitangent, nUp);
	}

	nRight = DirectX::XMVectorScale(nRight, scale);
	nUp = DirectX::XMVectorScale(nUp, scale);
	const auto& pos = DirectX::XMVectorAdd(position, DirectX::XMVectorScale(normal, 0.1f));
	DirectX::XMStoreFloat3(&vertices.at(0).position, DirectX::XMVectorAdd(DirectX::XMVectorAdd(pos, nRight), nUp));
	DirectX::XMStoreFloat3(&vertices.at(1).position, DirectX::XMVectorSubtract(DirectX::XMVectorAdd(pos, nRight), nUp));
	DirectX::XMStoreFloat3(&vertices.at(2).position, DirectX::XMVectorSubtract(DirectX::XMVectorSubtract(pos, nRight), nUp));
	DirectX::XMStoreFloat3(&vertices.at(3).position, DirectX::XMVectorAdd(DirectX::XMVectorSubtract(pos, nRight), nUp));

	mVertexBuffer.Create(renderer, vertices);

	VertexLayout layout{VertexLayout::Position3D, VertexLayout::Normal, VertexLayout::Tangent, VertexLayout::Bitangent, VertexLayout::Texture2D };

	pVertexShader = VertexShader::Resolve(renderer, "cso\\LocalSpacePlane_VS", layout);
}



WorldSpacePlane::WorldSpacePlane(Rendering & renderer, std::vector<std::string_view> texturePaths, UINT numOfInstances)
	:Plane(renderer)
{

	descriptor.numOfInstances = numOfInstances;
	descriptor.numOfTextures = texturePaths.size();

	float x = 0;
	float z = 0;

	matPositions.reserve(descriptor.numOfInstances);

	for (UINT i = 0; i < descriptor.numOfInstances; i++)
	{
		matPositions.emplace_back(DirectX::XMMatrixTranspose(DirectX::XMMatrixTranslation(x, 0.f, z)));
		
		x += 1;
		if (x > 5)
		{
			z += 0.1f;
			x = 0;
		}
	}

	vsTransformsCBuf.Create(renderer, 2, matPositions.size() * sizeof(DirectX::XMMATRIX), matPositions.data());
	
	VertexLayout layout{ VertexLayout::Position3D, VertexLayout::Normal, VertexLayout::Tangent, VertexLayout::Bitangent, VertexLayout::Texture2D };

	mVertexShader = VertexShader::Resolve(renderer, "cso\\PlaneWorldSpace_VS", layout);

	mVertexBuffer.CreateConst(renderer, vertices.size() * sizeof(PlaneVertex), sizeof(PlaneVertex), vertices.data());

	mPixelShader = PixelShader::Resolve(renderer, "cso\\PlaneWorldSpace_PS");

	if (descriptor.numOfTextures > 1)
	{
		mTextureArray = TextureArray::Resolve(renderer, texturePaths, 1u);
	}
	else
	{
		mTexture = Texture::Resolve(renderer, texturePaths.front(), 0u);
	}

	bool activeAlpha = true;
	mBlendState = &Blender::Get(renderer, activeAlpha, true);
	mRasterState = &Rasterizer::Get(renderer, activeAlpha);

	vspsDescCBuffer = std::make_unique<VSPSConstBuffer<DescCBuffer>>(renderer, 3u, descriptor);

}


Billboard::Billboard(Rendering & renderer, std::vector<DirectX::XMFLOAT3>& positions, std::vector<std::string_view> texturePaths, float scale)
	: descriptor{ (UINT)positions.size(), (UINT)texturePaths.size() },
	vertexBuffer{ renderer, sizeof(Vertex), sizeof(Vertex) * positions.size() }
{

	for (auto& pos : positions)
	{
		vertices.emplace_back(Vertex{ std::move(pos), { scale,scale } });
	}

	vertexBuffer.Update(renderer, vertices.data());

	VertexLayout layout{ VertexLayout::Position3D, VertexLayout::Position2D/*Size*/ };

	vertexShader = VertexShader::Resolve(renderer, "cso\\Billboard_VS", layout);

	pixelShader = PixelShader::Resolve(renderer, "cso\\BillboardArray_PS");

	geometryShader = GeometryShader::Resolve(renderer, "cso\\GSBillboardArray");

	textureArray = TextureArray::Resolve(renderer, texturePaths, 0u);
	bool hasActiveAlpha = textureArray->HasActiveAlpha();
	
	blendState = &Blender::Get(renderer, true);
	rasterState = &Rasterizer::Get(renderer, true);
	
	vspsgsCBuffer = std::make_unique<VSPSGSConstBuffer<DescCBuffer>>(renderer, 1u, descriptor);

}



Billboard::Billboard(Rendering & renderer, const DirectX::XMFLOAT3& position, std::string_view path, float scale, std::string_view pixelShaderPath)
	: vertexBuffer{renderer, sizeof(Vertex), sizeof(Vertex)}
{
	vertices.emplace_back(Vertex{ std::move(position), { scale,scale } });
	
	vertexBuffer.Update(renderer, vertices.data());

	VertexLayout layout{ VertexLayout::Position3D, VertexLayout::Position2D/*Size*/ };

	vertexShader = VertexShader::Resolve(renderer, "cso\\Billboard_VS", layout);

	pixelShader = PixelShader::Resolve(renderer, pixelShaderPath.data());

	geometryShader = GeometryShader::Resolve(renderer, "cso\\GSBillboard");

	texture = std::make_shared<Texture>(renderer, path, 0u);
	bool hasActiveAlpha = texture->HasActiveAlpha();
	
	blendState = &Blender::Get(renderer, true);
	rasterState = &Rasterizer::Get(renderer, hasActiveAlpha);

}

void Billboard::Bind(Rendering & renderer)
{
	vertexShader->Bind(renderer);
	vertexBuffer.Bind(renderer);
	Topology::Get(renderer, D3D11_PRIMITIVE_TOPOLOGY_POINTLIST).Bind(renderer);

	pixelShader->Bind(renderer);
	if (texture) texture->Bind(renderer);
	else textureArray->Bind(renderer);
	blendState->Bind(renderer);
	rasterState->Bind(renderer);

	Sampler::Get(renderer).Bind(renderer);

	geometryShader->Bind(renderer);

	if (vspsgsCBuffer) vspsgsCBuffer->Bind(renderer);
}
