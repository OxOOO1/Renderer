#pragma once


#include "Bindable/BindablesH.h"

//TODO Redesign 

class Plane
{
public:

	Plane(Rendering& renderer);

	bool bUseDebugDrawMode = false;

protected:
	////////////////Bindables//////////////
	IndexBuffer mIndexbuffer;
	Topology& mTopologyType;
	Sampler& mSamplerState;

	void Bind(Rendering& renderer)
	{
		mIndexbuffer.Bind(renderer);
		mTopologyType.Bind(renderer);
		mSamplerState.Bind(renderer);
	}

	struct PlaneVertex
	{
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT3 normal;
		DirectX::XMFLOAT3 tangent;
		DirectX::XMFLOAT3 bitangent;
		DirectX::XMFLOAT2 texCoord;
	};

	struct DescCBuffer
	{
		UINT numOfInstances = 1;
		UINT numOfTextures = 1;
		float pad1 = 0.f, 
			pad2 = 0.f;
	} descriptor;

	std::vector<PlaneVertex> vertices = {

		{	{ 1.f,  1.f, 0.0f}, {0.f, 0.f, 1.f}, {-1.f, 0.f, 0.f}, {0.f, 1.f, 0.f}, {1.0f, 1.0f},  },
		{	{ 1.f, -1.f, 0.0f}, {0.f, 0.f, 1.f}, {-1.f, 0.f, 0.f}, {0.f, 1.f, 0.f},	{1.0f, 0.0f},  },
		{	{-1.f, -1.f, 0.0f}, {0.f, 0.f, 1.f}, {-1.f, 0.f, 0.f}, {0.f, 1.f, 0.f},	{0.0f, 0.0f},  },
		{	{-1.f,  1.f, 0.0f},	{0.f, 0.f, 1.f}, {-1.f, 0.f, 0.f}, {0.f, 1.f, 0.f}, {0.0f, 1.0f},  }

	};

	std::vector<unsigned short> indices =
	{
	  2, 3, 0,
	  1, 2, 0
	};

};

class LocalSpacePlane : public Plane
{
public:

	LocalSpacePlane(Rendering& renderer, const DirectX::XMVECTOR& position, const DirectX::XMVECTOR& normal, float scale = 1.0f);

	void Draw(Rendering& renderer)
	{
		Plane::Bind(renderer);

		pVertexShader->Bind(renderer);
		mInputLayout.Bind(renderer);
		mVertexBuffer.Bind(renderer);

		renderer.GetContext()->DrawIndexed((UINT)indices.size(), 0, 0);
	}

	std::shared_ptr<VertexShader> pVertexShader;
	InputLayout mInputLayout;
	TVertexBuffer<PlaneVertex> mVertexBuffer;

};



class WorldSpacePlane : public Plane
{
public:

	WorldSpacePlane(Rendering& renderer, std::vector<std::string_view> texturePaths, UINT numOfInstances);

	void Draw(Rendering& renderer)
	{
		Plane::Bind(renderer);

		mVertexShader->Bind(renderer);
		mVertexBuffer.Bind(renderer);

		mPixelShader->Bind(renderer);

		if (mTexture) mTexture->Bind(renderer);
		else mTextureArray->Bind(renderer);
		mBlendState->Bind(renderer);
		mRasterState->Bind(renderer);

		vsTransformsCBuf.Bind(renderer);

		vspsDescCBuffer->Bind(renderer);

		renderer.GetContext()->DrawIndexedInstanced((UINT)indices.size(), descriptor.numOfInstances, 0,0,0);
	}

	std::shared_ptr<VertexShader> mVertexShader;
	VertexBuffer mVertexBuffer;

	std::shared_ptr<PixelShader> mPixelShader;
	std::shared_ptr<Texture> mTexture;
	std::shared_ptr<TextureArray> mTextureArray;
	Blender* mBlendState;
	Rasterizer* mRasterState;

	std::vector<DirectX::XMMATRIX> matPositions;

	VertexConstBuffer<DirectX::XMMATRIX> vsTransformsCBuf;
	
	std::unique_ptr<VSPSConstBuffer<DescCBuffer>> vspsDescCBuffer;
};



class Billboard
{
public:

	Billboard(Rendering& renderer, std::vector<DirectX::XMFLOAT3>& positions, std::vector<std::string_view> texturePaths, float scale = 1.0f);

	Billboard(Rendering& renderer, const DirectX::XMFLOAT3& position, std::string_view path, float scale = 1.0f, std::string_view pixelShader = "cso\\Billboard_PS");

	void Draw(Rendering& renderer) noexcept 
	{
		Bind(renderer);

		renderer.GetContext()->Draw(descriptor.numOfInstances, 0);

		Unbind(renderer);
	}

	void SetPosition(Rendering& renderer, const DirectX::XMFLOAT3& newPos, UINT at = 0)
	{
		vertices.at(at).position = newPos;
		vertexBuffer.Update(renderer, vertices.data());
	}

	void SetScale(Rendering& renderer, DirectX::XMFLOAT2 scale)
	{
		vertices.front().size = scale;
		vertexBuffer.Update(renderer, vertices.data());
	}
	
private:

	//////////////// Bindables ///////////

	void Bind(Rendering& renderer);

	void Unbind(Rendering& renderer)
	{
		geometryShader->SetNull(renderer);
	}

	std::shared_ptr<VertexShader> vertexShader;
	struct Vertex
	{
		DirectX::XMFLOAT3 position{ 0.f,0.f,0.f };
		DirectX::XMFLOAT2 size{ 1.f,1.f };
	};
	std::vector<Vertex> vertices;

	DynVertexBuffer vertexBuffer;

	std::shared_ptr<PixelShader> pixelShader;
	std::shared_ptr<Texture> texture;
	std::shared_ptr<TextureArray> textureArray;
	Blender* blendState;
	Rasterizer* rasterState;

	std::shared_ptr<GeometryShader> geometryShader;
	
	struct DescCBuffer
	{
		UINT numOfInstances = 1;
		UINT numOfTextures = 1;
		float padding = 0;
		float padding2 = 0;
		DirectX::XMMATRIX pad;
	} descriptor;

	std::unique_ptr<VSPSGSConstBuffer<DescCBuffer>> vspsgsCBuffer;

};
