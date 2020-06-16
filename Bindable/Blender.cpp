#include "Blender.h"
#include "TexturesShadersPool.h"


int Blender::currentlyBoundId = -1;


Blender::Blender(Rendering & renderer, bool Enabled, bool AlphaToCoverage)
{
	D3D11_BLEND_DESC blendDesc = {};
	auto& brt = blendDesc.RenderTarget[0]; 
	if (Enabled)
	{
		id = 0;

		brt.BlendEnable = TRUE;
		brt.SrcBlend = D3D11_BLEND_SRC_ALPHA;
		brt.DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
		brt.BlendOp = D3D11_BLEND_OP_ADD;
		brt.SrcBlendAlpha = D3D11_BLEND_ZERO;
		brt.DestBlendAlpha = D3D11_BLEND_ZERO;
		brt.BlendOpAlpha = D3D11_BLEND_OP_ADD;
		brt.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
		if (AlphaToCoverage)
		{
			id += 1;
			blendDesc.AlphaToCoverageEnable = true;
		}
		
	}
	else
	{
		id = 2;

		brt.BlendEnable = FALSE;
		brt.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
		if (AlphaToCoverage)
		{
			id += 1;
			blendDesc.AlphaToCoverageEnable = true;
		}
			
	}

	renderer.ThrowIfFailed(renderer.GetDevice()->CreateBlendState(&blendDesc, &pBlender));
}


void Blender::CreateForParticles(Rendering& renderer, D3D11_BLEND_OP blendOp, bool alphaPremult)
{
	if (!isInit)
	{
		id = 3 + blendOp;

		D3D11_BLEND_DESC blendDesc = {};
		auto& brt = blendDesc.RenderTarget[0];
		brt.BlendEnable = TRUE;
		brt.SrcBlendAlpha = D3D11_BLEND_ZERO;
		brt.DestBlendAlpha = D3D11_BLEND_ZERO;
		brt.BlendOpAlpha = D3D11_BLEND_OP_MAX;
		brt.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

		brt.SrcBlend = D3D11_BLEND_SRC_ALPHA;
		brt.DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
		brt.BlendOp = blendOp;


		renderer.ThrowIfFailed(renderer.GetDevice()->CreateBlendState(&blendDesc, &pBlender));

		isInit = true;
	}
	
}


void Blender::Bind(Rendering & renderer) const
{
	if (id != currentlyBoundId)
	{
		currentlyBoundId = id;
		renderer.GetContext()->OMSetBlendState(pBlender.Get(), nullptr, 0xFFFFFFFFu);
	}
}
