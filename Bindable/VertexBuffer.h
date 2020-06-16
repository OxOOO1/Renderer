#pragma once


#include "RawVertexBuffer.h"
#include "System/Rendering.h"

#include <cstdarg>

class VertexBuffer
{
public:
	
	void Create(Rendering & renderer, UINT size, UINT vertexSize);
	void Create(Rendering& renderer, const RawVertexBuffer& vbuf);
	void CreateConst(Rendering & renderer, UINT size, UINT vertexSize, const void* data);
	
	void CreateStreamOutput(Rendering& renderer, const RawVertexBuffer& vbuf);
	void CreateStreamOutput(Rendering & renderer, UINT size, UINT vertexSize);
	

	void Bind(Rendering& renderer, UINT slot = 0) const;

	void BindSO(Rendering& renderer)
	{
		renderer.GetContext()->SOSetTargets(1u, pVertexBuffer.GetAddressOf(), &offset);
	}

	void UnbindSO(Rendering& renderer)
	{
		renderer.GetContext()->SOSetTargets(0u, nullptr, 0u);
	}

	void Copy(Rendering& renderer, Microsoft::WRL::ComPtr<ID3D11Buffer>& destination)
	{
		renderer.GetContext()->CopyResource(destination.Get(), pVertexBuffer.Get());
	}

	Microsoft::WRL::ComPtr<ID3D11Buffer>& GetResource()
	{
		return pVertexBuffer;
	}

	template<size_t Amount>
	static void BindVertexBuffers(Rendering& renderer, const std::vector<VertexBuffer>& buffers)
	{
		ID3D11Buffer* pBuffers[Amount];
		UINT pStrides[Amount];
		UINT pOffsets[Amount];

		for (int i = 0; i < Amount; i++){
			pBuffers[i] = buffers[i].pVertexBuffer.Get();
			pStrides[i] = buffers[i].stride;
			pOffsets[i] = buffers[i].offset;
		}

		renderer.GetContext()->IASetVertexBuffers(0u, Amount, pBuffers, pStrides, pOffsets);
	}
	
protected:

		Microsoft::WRL::ComPtr<ID3D11Buffer> pVertexBuffer;
		UINT stride = 0;
		UINT offset = 0;
};

template<typename T>
class TVertexBuffer
{
public:

	TVertexBuffer()
		: stride((UINT)sizeof(T))
	{
	}

	void Create(Rendering & renderer, const T& vbuf)
	{
		D3D11_BUFFER_DESC bd = {};
		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.CPUAccessFlags = 0u;
		bd.MiscFlags = 0u;
		bd.ByteWidth = UINT(sizeof(T));
		bd.StructureByteStride = 0u;//0u

		//Vertex buffer data
		D3D11_SUBRESOURCE_DATA sd = {};
		sd.pSysMem = &vbuf;

		renderer.ThrowIfFailed(renderer.GetDevice()->CreateBuffer(&bd, &sd, &pVertexBuffer));
	}

	void Create(Rendering & renderer, const std::vector<T>& vbuf)
	{

		D3D11_BUFFER_DESC bd = {};
		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.CPUAccessFlags = 0u;
		bd.MiscFlags = 0u;
		bd.ByteWidth = UINT(sizeof(T) * vbuf.size());
		bd.StructureByteStride = 0u;//0u

		//Vertex buffer data
		D3D11_SUBRESOURCE_DATA sd = {};
		sd.pSysMem = vbuf.data();

		renderer.ThrowIfFailed(renderer.GetDevice()->CreateBuffer(&bd, &sd, &pVertexBuffer));

	}

	//For struct that contains array
	TVertexBuffer(Rendering & renderer, const T& vbuf)
		: stride((UINT)sizeof(T))
	{

		D3D11_BUFFER_DESC bd = {};
		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.CPUAccessFlags = 0u;
		bd.MiscFlags = 0u;
		bd.ByteWidth = UINT(sizeof(T));
		bd.StructureByteStride = 0u;//0u

		//Vertex buffer data
		D3D11_SUBRESOURCE_DATA sd = {};
		sd.pSysMem = &vbuf;

		renderer.ThrowIfFailed(renderer.GetDevice()->CreateBuffer(&bd, &sd, &pVertexBuffer));

	}

	//For array containing struct
	TVertexBuffer(Rendering & renderer, const std::vector<T>& vbuf)
		: stride((UINT)sizeof(T))
	{

		D3D11_BUFFER_DESC bd = {};
		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.CPUAccessFlags = 0u;
		bd.MiscFlags = 0u;
		bd.ByteWidth = UINT(sizeof(T) * vbuf.size());
		bd.StructureByteStride = 0u;//0u

		//Vertex buffer data
		D3D11_SUBRESOURCE_DATA sd = {};
		sd.pSysMem = vbuf.data();

		renderer.ThrowIfFailed(renderer.GetDevice()->CreateBuffer(&bd, &sd, &pVertexBuffer));
		
	}

	void Bind(Rendering& renderer) const
	{
		renderer.GetContext()->IASetVertexBuffers(0u, 1u, pVertexBuffer.GetAddressOf(), &stride, &offset);
	}

private:
	Microsoft::WRL::ComPtr<ID3D11Buffer> pVertexBuffer;
	UINT stride = 0;
	UINT offset = 0;
};

class DynVertexBuffer
{
public:

	DynVertexBuffer(Rendering& renderer, UINT vertexSize, UINT bufferSize);

	void Bind(Rendering& renderer) const;

	template<typename T>
	void Update(Rendering& renderer, T* data)
	{
		D3D11_MAPPED_SUBRESOURCE msr;
		renderer.ThrowIfFailed(renderer.GetContext()->Map(pVertexBuffer.Get(), 0u, D3D11_MAP_WRITE_DISCARD, 0u, &msr));

		std::memcpy(msr.pData, data, bufferSize);

		renderer.GetContext()->Unmap(pVertexBuffer.Get(), 0u);
	}

private:
	Microsoft::WRL::ComPtr<ID3D11Buffer> pVertexBuffer;
	UINT stride = 0;
	UINT offset = 0;
	UINT bufferSize = 0;
};