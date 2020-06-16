#pragma once
#include "System/Rendering.h"
#include "VertexLayout.h"

class InputLayout
{
public:

	void Create(Rendering& renderer, VertexLayout layout, ID3DBlob* bytecode);
	
	void Create(Rendering & renderer, const std::vector<D3D11_INPUT_ELEMENT_DESC> descs, ID3DBlob * bytecode);

	void Bind(Rendering& renderer) const;

private:
	
	Microsoft::WRL::ComPtr<ID3D11InputLayout> pInputLayout;
};