#include "IndexBuffer.h"


void IndexBuffer::Create(Rendering & renderer, const std::vector<unsigned short>& indices)
{
	count = (UINT)indices.size();
	D3D11_BUFFER_DESC ibd = {};
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.Usage = D3D11_USAGE_DEFAULT;
	ibd.CPUAccessFlags = 0u;
	ibd.MiscFlags = 0u;
	ibd.ByteWidth = UINT(sizeof(unsigned short) * count);
	ibd.StructureByteStride = sizeof(unsigned short);

	D3D11_SUBRESOURCE_DATA isd = {};
	isd.pSysMem = indices.data();
	renderer.ThrowIfFailed(renderer.GetDevice()->CreateBuffer(&ibd, &isd, &pIndexBuffer));
}

void IndexBuffer::Create(Rendering & renderer, const std::string & tag, const std::vector<unsigned short>& indices)
{
	count = (UINT)indices.size();
	D3D11_BUFFER_DESC ibd = {};
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.Usage = D3D11_USAGE_DEFAULT;
	ibd.CPUAccessFlags = 0u;
	ibd.MiscFlags = 0u;
	ibd.ByteWidth = UINT(sizeof(unsigned short) * count);
	ibd.StructureByteStride = sizeof(unsigned short);

	D3D11_SUBRESOURCE_DATA isd = {};
	isd.pSysMem = indices.data();
	renderer.ThrowIfFailed(renderer.GetDevice()->CreateBuffer(&ibd, &isd, &pIndexBuffer));
}

 void IndexBuffer::Bind(Rendering & renderer) const
{
	renderer.GetContext()->IASetIndexBuffer(pIndexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0);
}
