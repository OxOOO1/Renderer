#include "DepthStencilState.h"
#include "System/Rendering.h"
DepthStencilState::DepthStencilState(Rendering & renderer, Mode mode)
{
	D3D11_DEPTH_STENCIL_DESC dsDesc = CD3D11_DEPTH_STENCIL_DESC{ CD3D11_DEFAULT{} };
	
	if (mode == Mode::Write)
	{
		dsDesc.StencilEnable = TRUE;
		dsDesc.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;
		dsDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
		dsDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_REPLACE;
	}
	else if (mode == Mode::Mask)
	{
		dsDesc.DepthEnable = FALSE;
		dsDesc.StencilEnable = TRUE;
		dsDesc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
		dsDesc.FrontFace.StencilFunc = D3D11_COMPARISON_NOT_EQUAL;

	}
	else if (mode == Mode::NoDepth)
	{
		dsDesc.DepthEnable = FALSE;
		dsDesc.DepthFunc = D3D11_COMPARISON_ALWAYS;
		dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		
	}
	else if (mode == Mode::DepthNoWrites)
	{
		dsDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
		dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	}

	renderer.GetDevice()->CreateDepthStencilState(&dsDesc, &pStencil);

}

void DepthStencilState::Bind(Rendering & renderer) const
{
	renderer.GetContext()->OMSetDepthStencilState(pStencil.Get(), 0xFF);
}
