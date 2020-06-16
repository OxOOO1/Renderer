#include "Sampler.h"



Sampler::Sampler(Rendering & renderer, D3D11_FILTER filterMode)
{

	D3D11_SAMPLER_DESC samplerDesc = {};
	samplerDesc.Filter = filterMode;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MinLOD = 0.0f;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
	renderer.ThrowIfFailed(renderer.GetDevice()->CreateSamplerState(&samplerDesc, &pSampler));
}

void Sampler::Bind(Rendering & renderer, UINT slot) const
{
	renderer.GetContext()->PSSetSamplers(slot, 1u, pSampler.GetAddressOf());
}

void Sampler::BindGS(Rendering & renderer, UINT slot) const
{
	renderer.GetContext()->GSSetSamplers(slot, 1u, pSampler.GetAddressOf());
}

void Sampler::BindVS(Rendering & renderer, UINT slot) const
{
	renderer.GetContext()->VSSetSamplers(slot, 1u, pSampler.GetAddressOf());
}

