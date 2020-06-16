#include "InputLayout.h"
#include "VertexShader.h"
#include "RawVertexBuffer.h"


void InputLayout::Create(Rendering & renderer, VertexLayout layout, ID3DBlob * bytecode)
{
	const auto layoutDesc = layout.GetD3DLayout();
	
	renderer.ThrowIfFailed(
		renderer.GetDevice()->CreateInputLayout(
			layoutDesc.data(), (UINT)layoutDesc.size(),
			bytecode->GetBufferPointer(), bytecode->GetBufferSize(),
			&pInputLayout)
	);
}

void InputLayout::Create(Rendering & renderer, const std::vector<D3D11_INPUT_ELEMENT_DESC> descs, ID3DBlob * bytecode) 
{
	renderer.ThrowIfFailed(
		renderer.GetDevice()->CreateInputLayout(
			descs.data(), (UINT)descs.size(),
			bytecode->GetBufferPointer(), bytecode->GetBufferSize(),
			&pInputLayout)
	);
}

void InputLayout::Bind(Rendering & renderer) const
{
	renderer.GetContext()->IASetInputLayout(pInputLayout.Get());
}

