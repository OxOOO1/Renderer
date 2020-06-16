#pragma once

#include "System/Rendering.h"

class StreamOutput
{
public:

	StreamOutput(Rendering& renderer, UINT size, UINT bindFlags = 0u);

	void Copy(Rendering& renderer, Microsoft::WRL::ComPtr<ID3D11Buffer>& to)
	{
		renderer.GetContext()->CopyResource(to.Get(), pStreamOutput.Get());
	}

	void Copy(Rendering& renderer, char* to, UINT size)
	{
		D3D11_MAPPED_SUBRESOURCE msr;
		renderer.ThrowIfFailed(renderer.GetContext()->Map(pStreamOutput.Get(), 0u, D3D11_MAP_READ, 0u, &msr));

		std::memcpy(to, msr.pData, size);

		renderer.GetContext()->Unmap(pStreamOutput.Get(), 0u);
	}

	void Bind(Rendering& renderer) const 
	{
		renderer.GetContext()->SOSetTargets(1u, pStreamOutput.GetAddressOf(), &offset);
	}

	void UnBind(Rendering& renderer)
	{
		renderer.GetContext()->SOSetTargets(0u, nullptr, 0u);
	}

	Microsoft::WRL::ComPtr<ID3D11Buffer>& GetResource()
	{
		return pStreamOutput;
	}

protected:
	
	Microsoft::WRL::ComPtr<ID3D11Buffer> pStreamOutput;
	UINT offset = 0;
	
};
