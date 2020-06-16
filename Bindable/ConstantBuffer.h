#pragma once


#include "System/Rendering.h"

//GPU CBuffer;

template<typename T>
class CBuffer
{
public:
	CBuffer() noexcept = default;

	CBuffer(Rendering& renderer, UINT _slot, const T& _data)
		: slot(_slot)
	{

		D3D11_BUFFER_DESC cbd = {};
		cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		cbd.Usage = D3D11_USAGE_DYNAMIC;
		cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		cbd.MiscFlags = 0u;
		cbd.ByteWidth = sizeof(_data); //T * std::size(_data));
		cbd.StructureByteStride = 0u;

		D3D11_SUBRESOURCE_DATA csd = {};
		csd.pSysMem = &_data;
		renderer.ThrowIfFailed(renderer.GetDevice()->CreateBuffer(&cbd, &csd, &pConstantBuffer));

	}
	
	CBuffer(Rendering& renderer, UINT _slot = 1u) : slot(_slot)
	{
		D3D11_BUFFER_DESC cbd = {};
		cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		cbd.Usage = D3D11_USAGE_DYNAMIC;
		cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		cbd.MiscFlags = 0u;
		cbd.ByteWidth = sizeof(T); 
		cbd.StructureByteStride = 0u;

		renderer.ThrowIfFailed(renderer.GetDevice()->CreateBuffer(&cbd, nullptr, &pConstantBuffer));
	}

	CBuffer(Rendering& renderer, UINT size, UINT _slot) : slot(_slot)
	{
		D3D11_BUFFER_DESC cbd = {};
		cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		cbd.Usage = D3D11_USAGE_DYNAMIC;
		cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		cbd.MiscFlags = 0u;
		cbd.ByteWidth = size; 
		cbd.StructureByteStride = 0;
		
		renderer.ThrowIfFailed(renderer.GetDevice()->CreateBuffer(&cbd, nullptr, &pConstantBuffer));
	}

	void Create(Rendering& renderer, UINT _slot, UINT sizeBytes, const T* _data)
	{
		slot = _slot;
		capacity = sizeBytes;
		size = sizeBytes / sizeof(T);

		D3D11_BUFFER_DESC cbd = {};
		cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		cbd.Usage = D3D11_USAGE_DYNAMIC;
		cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		cbd.MiscFlags = 0u;
		cbd.ByteWidth = sizeBytes;
		cbd.StructureByteStride = 0;

		D3D11_SUBRESOURCE_DATA csd = {};
		csd.pSysMem = _data;
		renderer.ThrowIfFailed(renderer.GetDevice()->CreateBuffer(&cbd, &csd, &pConstantBuffer));
	}

	void Create(Rendering& renderer, UINT _slot, UINT sizeBytes)
	{
		slot = _slot;
		capacity = sizeBytes;
		size = sizeBytes / sizeof(T);

		D3D11_BUFFER_DESC cbd = {};
		cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		cbd.Usage = D3D11_USAGE_DYNAMIC;
		cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		cbd.MiscFlags = 0u;
		cbd.ByteWidth = sizeBytes;
		cbd.StructureByteStride = 0;

		renderer.ThrowIfFailed(renderer.GetDevice()->CreateBuffer(&cbd, nullptr, &pConstantBuffer));
	}

	
	void Update(Rendering& renderer, const T& data) const
	{
		D3D11_MAPPED_SUBRESOURCE msr;
		
		renderer.ThrowIfFailed(renderer.GetContext()->Map(pConstantBuffer.Get(), 0u, D3D11_MAP_WRITE_DISCARD, 0u, &msr));

		std::memcpy(msr.pData, &data, sizeof(data));
		
		renderer.GetContext()->Unmap(pConstantBuffer.Get(), 0u);

	}

	template<typename D>
	void Update(Rendering& renderer, const D* data, UINT dataSize) const
	{

		assert(dataSize <= capacity);

		D3D11_MAPPED_SUBRESOURCE msr;
		renderer.ThrowIfFailed(renderer.GetContext()->Map(pConstantBuffer.Get(), 0u, D3D11_MAP_WRITE_DISCARD, 0u, &msr));
		
		std::memcpy(msr.pData, data, dataSize);

		renderer.GetContext()->Unmap(pConstantBuffer.Get(), 0u);

	}

	UINT Size()
	{
		return size;
	}

	UINT SizeBytes()
	{
		return capacity;
	}

protected:
	
	Microsoft::WRL::ComPtr<ID3D11Buffer> pConstantBuffer;
	UINT slot = 0;
	UINT capacity = 0;
	UINT size = 0;
};


template<typename T>
class VertexConstBuffer : public CBuffer<T>
{
public:

	using CBuffer<T>::pConstantBuffer;
	using CBuffer<T>::slot;

public:
	using CBuffer<T>::CBuffer;

	void Bind(Rendering& renderer) const
	{
		renderer.GetContext()->VSSetConstantBuffers(slot, 1u, pConstantBuffer.GetAddressOf());
	}
	void Bind(Rendering& renderer, UINT slot) const
	{
		renderer.GetContext()->VSSetConstantBuffers(slot, 1u, pConstantBuffer.GetAddressOf());
	}
	

};

template<typename T>
class PixelConstBuffer : public CBuffer<T>
{
public:

	using CBuffer<T>::pConstantBuffer;
	using CBuffer<T>::slot;
public:
	using CBuffer<T>::CBuffer;

	void Bind(Rendering& renderer) const
	{
		renderer.GetContext()->PSSetConstantBuffers(slot, 1u, pConstantBuffer.GetAddressOf());
	}
};

template<typename T>
class ConstCBuffer
{
public:
	ConstCBuffer() noexcept = default;

	//For custom structs
	void Create(Rendering& renderer, UINT _slot, const T& _data)
	{
		slot = _slot;
		D3D11_BUFFER_DESC cbd = {};
		cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		cbd.Usage = D3D11_USAGE_IMMUTABLE;
		cbd.CPUAccessFlags = 0u;
		cbd.MiscFlags = 0u;
		cbd.ByteWidth = sizeof(_data); 
		cbd.StructureByteStride = 0u;

		D3D11_SUBRESOURCE_DATA csd = {};
		csd.pSysMem = &_data;
		renderer.ThrowIfFailed(renderer.GetDevice()->CreateBuffer(&cbd, &csd, &pConstantBuffer));
	}

	//for T arrays
	void Create(Rendering& renderer, UINT _slot, UINT sizeBytes, const T* _data)
	{
		slot = _slot;
		D3D11_BUFFER_DESC cbd = {};
		cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		cbd.Usage = D3D11_USAGE_IMMUTABLE;
		cbd.CPUAccessFlags = 0u;
		cbd.MiscFlags = 0u;
		cbd.ByteWidth = sizeBytes; 
		cbd.StructureByteStride = 0u;

		size = sizeBytes / sizeof(T);

		D3D11_SUBRESOURCE_DATA csd = {};
		csd.pSysMem = _data;
		renderer.ThrowIfFailed(renderer.GetDevice()->CreateBuffer(&cbd, &csd, &pConstantBuffer));
	}

	UINT Size()
	{
		return size;
	}

protected:

	Microsoft::WRL::ComPtr<ID3D11Buffer> pConstantBuffer;
	UINT slot = 0;
	UINT size = 0;
};


template<typename T>
class ConstPixelCBuffer : public ConstCBuffer<T>
{
public:

	using ConstCBuffer<T>::pConstantBuffer;
	using ConstCBuffer<T>::slot;
public:
	using ConstCBuffer<T>::ConstCBuffer;

	using ConstCBuffer<T>::Create;

	void Bind(Rendering& renderer) const
	{
		renderer.GetContext()->PSSetConstantBuffers(slot, 1u, pConstantBuffer.GetAddressOf());
	}
};

template<typename T>
class ConstVertexCBuffer : public ConstCBuffer<T>
{
public:

	using ConstCBuffer<T>::pConstantBuffer;
	using ConstCBuffer<T>::slot;
public:
	using ConstCBuffer<T>::ConstCBuffer;

	using ConstCBuffer<T>::Create;

	void Bind(Rendering& renderer) const
	{
		renderer.GetContext()->VSSetConstantBuffers(slot, 1u, pConstantBuffer.GetAddressOf());
	}
	void Bind(Rendering& renderer, UINT slot) const
	{
		renderer.GetContext()->VSSetConstantBuffers(slot, 1u, pConstantBuffer.GetAddressOf());
	}
};

template<typename T>
class ConstComputeCBuffer : public ConstCBuffer<T>
{
public:

	using ConstCBuffer<T>::pConstantBuffer;
	using ConstCBuffer<T>::slot;
public:
	using ConstCBuffer<T>::ConstCBuffer;

	using ConstCBuffer<T>::Create;

	void Bind(Rendering& renderer) const
	{
		renderer.GetContext()->CSSetConstantBuffers(slot, 1u, pConstantBuffer.GetAddressOf());
	}
	void Bind(Rendering& renderer, UINT slot) const
	{
		renderer.GetContext()->CSSetConstantBuffers(slot, 1u, pConstantBuffer.GetAddressOf());
	}
};

template<typename T>
class VSPSConstBuffer : public CBuffer<T>
{
public:

	using CBuffer<T>::pConstantBuffer;

	using CBuffer<T>::slot;
public:
	using CBuffer<T>::CBuffer;

	void Bind(Rendering& renderer) const
	{
		renderer.GetContext()->VSSetConstantBuffers(slot, 1u, pConstantBuffer.GetAddressOf());
		renderer.GetContext()->PSSetConstantBuffers(slot, 1u, pConstantBuffer.GetAddressOf());
	}
};

template<typename T>
class GeometryConstBuffer : public CBuffer<T>
{
public:

	using CBuffer<T>::pConstantBuffer;

	using CBuffer<T>::slot;
public:
	using CBuffer<T>::CBuffer;

	void Bind(Rendering& renderer) const
	{
		renderer.GetContext()->GSSetConstantBuffers(slot, 1u, pConstantBuffer.GetAddressOf());
	}
};

template<typename T>
class ComputeConstBuffer : public CBuffer<T>
{
public:

	using CBuffer<T>::pConstantBuffer;

	using CBuffer<T>::slot;
public:
	using CBuffer<T>::CBuffer;

	void Bind(Rendering& renderer) const
	{
		renderer.GetContext()->CSSetConstantBuffers(slot, 1u, pConstantBuffer.GetAddressOf());
	}
};

template<typename T>
class VSPSGSConstBuffer : public CBuffer<T>
{
public:

	using CBuffer<T>::pConstantBuffer;

	using CBuffer<T>::slot;
public:
	using CBuffer<T>::CBuffer;

	void Bind(Rendering& renderer) const
	{
		renderer.GetContext()->VSSetConstantBuffers(slot, 1u, pConstantBuffer.GetAddressOf());
		renderer.GetContext()->PSSetConstantBuffers(slot, 1u, pConstantBuffer.GetAddressOf());
		renderer.GetContext()->GSSetConstantBuffers(slot, 1u, pConstantBuffer.GetAddressOf());
	}
};

template<typename T>
class VSGSConstBuffer : public CBuffer<T>
{
public:

	using CBuffer<T>::pConstantBuffer;

	using CBuffer<T>::slot;
public:
	using CBuffer<T>::CBuffer;

	void Bind(Rendering& renderer) const
	{
		renderer.GetContext()->VSSetConstantBuffers(slot, 1u, pConstantBuffer.GetAddressOf());
		renderer.GetContext()->GSSetConstantBuffers(slot, 1u, pConstantBuffer.GetAddressOf());
	}
};