#include "Rasterizer.h"

Rasterizer::Rasterizer(Rendering & renderer, D3D11_CULL_MODE mode, bool depthBias)
{
	if (!depthBias)
	{
		D3D11_RASTERIZER_DESC raDesc = CD3D11_RASTERIZER_DESC(CD3D11_DEFAULT{});
		raDesc.CullMode = mode;
		renderer.ThrowIfFailed(renderer.GetDevice()->CreateRasterizerState(&raDesc, &pRasterState));
	}
	else
	{
		D3D11_RASTERIZER_DESC raDesc = CD3D11_RASTERIZER_DESC(CD3D11_DEFAULT{});
		raDesc.DepthBias = 1.f;
		raDesc.DepthBiasClamp = 0.0f;
		raDesc.SlopeScaledDepthBias = 2.f;
		raDesc.CullMode = mode;
		renderer.ThrowIfFailed(renderer.GetDevice()->CreateRasterizerState(&raDesc, &pRasterState));
	}
	 
}

void Rasterizer::Bind(Rendering & renderer) const
{
	renderer.GetContext()->RSSetState(pRasterState.Get());
}
