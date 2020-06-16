#include "VertexBuffer.h"

void VertexBuffer::Create(Rendering & renderer, const RawVertexBuffer & vbuf)
{
	CreateConst(renderer, vbuf.SizeBytes(), vbuf.GetVertexLayout().SizeBytes(), vbuf.GetDataConst());
}
void VertexBuffer::CreateConst(Rendering & renderer, UINT sizeBytes, UINT vertexSize, const void* data)
{
	stride = vertexSize;
	D3D11_BUFFER_DESC bd = {};
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.Usage = D3D11_USAGE_IMMUTABLE;
	bd.CPUAccessFlags = 0u;
	bd.MiscFlags = 0u;
	bd.ByteWidth = sizeBytes;
	bd.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA sd = {};
	sd.pSysMem = data;
	renderer.ThrowIfFailed(renderer.GetDevice()->CreateBuffer(&bd, &sd, &pVertexBuffer));
}

void VertexBuffer::Create(Rendering & renderer, UINT sizeBytes, UINT vertexSize)
{
	stride = vertexSize;
	D3D11_BUFFER_DESC bd = {};
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.CPUAccessFlags = 0u;
	bd.MiscFlags = 0u;
	bd.ByteWidth = sizeBytes;
	bd.StructureByteStride = 0;//0u

	renderer.ThrowIfFailed(renderer.GetDevice()->CreateBuffer(&bd, nullptr, &pVertexBuffer));
}

void VertexBuffer::CreateStreamOutput(Rendering & renderer, const RawVertexBuffer & vbuf)
{
	stride = (UINT)vbuf.GetVertexLayout().SizeBytes();
	D3D11_BUFFER_DESC bd = {};
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER | D3D11_BIND_STREAM_OUTPUT;
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.CPUAccessFlags = 0u;
	bd.MiscFlags = 0u;
	bd.ByteWidth = UINT(vbuf.SizeBytes());
	bd.StructureByteStride = 0;//0u

	//Vertex buffer data
	D3D11_SUBRESOURCE_DATA sd = {};
	sd.pSysMem = vbuf.GetDataConst();

	renderer.ThrowIfFailed(renderer.GetDevice()->CreateBuffer(&bd, &sd, &pVertexBuffer));

}

void VertexBuffer::CreateStreamOutput(Rendering & renderer, UINT sizeBytes, UINT vertexSize)
{
	stride = vertexSize;
	D3D11_BUFFER_DESC bd = {};
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER | D3D11_BIND_STREAM_OUTPUT;
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.CPUAccessFlags = 0u;
	bd.MiscFlags = 0u;
	bd.ByteWidth = sizeBytes;
	bd.StructureByteStride = 0;//0u

	renderer.ThrowIfFailed(renderer.GetDevice()->CreateBuffer(&bd, nullptr, &pVertexBuffer));
}

 void VertexBuffer::Bind(Rendering & renderer, UINT slot) const
{
	renderer.GetContext()->IASetVertexBuffers(slot, 1u, pVertexBuffer.GetAddressOf(), &stride, &offset);
}

DynVertexBuffer::DynVertexBuffer(Rendering & renderer, UINT vertexSize, UINT bufferSize)
	: stride(vertexSize), bufferSize(bufferSize)
{
	
	D3D11_BUFFER_DESC bd = {};
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bd.MiscFlags = 0u;
	bd.ByteWidth = bufferSize;
	bd.StructureByteStride = 0;

	renderer.ThrowIfFailed(renderer.GetDevice()->CreateBuffer(&bd, nullptr, &pVertexBuffer));
}

void DynVertexBuffer::Bind(Rendering & renderer) const
{
	renderer.GetContext()->IASetVertexBuffers(0u, 1u, pVertexBuffer.GetAddressOf(), &stride, &offset);
}
