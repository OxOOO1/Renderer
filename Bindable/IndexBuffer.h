#pragma once

#include "System/Rendering.h"
#include <vector>
class IndexBuffer
{
public:

	void Create(Rendering& renderer, const std::vector<unsigned short>& indices);
	void Create(Rendering& renderer, const std::string& tag, const std::vector<unsigned short>& indices);

	void Bind(Rendering& renderer) const ;

	UINT GetCount() const { return count; }

private:
	Microsoft::WRL::ComPtr<ID3D11Buffer> pIndexBuffer;
	UINT count = 0;
};