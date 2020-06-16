#pragma once
#include "System/Rendering.h"

class Sampler
{
public:

	static constexpr auto DefaultSamplingFilter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;

	
	static Sampler& Get(Rendering& renderer, D3D11_FILTER FilterMode = Sampler::DefaultSamplingFilter)
	{
		if (FilterMode == D3D11_FILTER_MIN_MAG_MIP_LINEAR)
		{
			static Sampler s1(renderer, (D3D11_FILTER)D3D11_FILTER_MIN_MAG_MIP_LINEAR);
			return s1;
		}
		if (FilterMode == D3D11_FILTER_MIN_MAG_MIP_POINT)
		{
			static Sampler s2(renderer, (D3D11_FILTER)D3D11_FILTER_MIN_MAG_MIP_POINT);
			return s2;
		}
		if (FilterMode == D3D11_FILTER_ANISOTROPIC)
		{
			static Sampler s3(renderer, (D3D11_FILTER)D3D11_FILTER_ANISOTROPIC);
			return s3;
		}

	}

	static Sampler& GetComparisonSampler(Rendering& renderer, bool comparisonLess = true)
	{
		if (comparisonLess)
		{
			static Sampler shadowless(renderer, D3D11_COMPARISON_LESS);
			return shadowless;
		}
		else
		{
			static Sampler shadowgreat(renderer, D3D11_COMPARISON_GREATER);
			return shadowgreat;
		}
		
	}

	void Bind(Rendering& renderer, UINT slot = 0u) const;
	void BindGS(Rendering& renderer, UINT slot = 0u) const;
	void BindVS(Rendering& renderer, UINT slot = 0u) const;

public:
	Sampler(Rendering& renderer, D3D11_FILTER filterMode, D3D11_TEXTURE_ADDRESS_MODE adressMode, D3D11_COMPARISON_FUNC compFunc= D3D11_COMPARISON_LESS, DirectX::XMFLOAT4 borderColor = { 1.,1.,1.,1. })
	{
		D3D11_SAMPLER_DESC samplerDesc = {};

		samplerDesc.Filter = filterMode;

		samplerDesc.AddressU = adressMode;
		samplerDesc.AddressV = adressMode;
		samplerDesc.AddressW = adressMode;

		samplerDesc.BorderColor[0] = borderColor.x;
		samplerDesc.BorderColor[1] = borderColor.y;
		samplerDesc.BorderColor[2] = borderColor.z;
		samplerDesc.BorderColor[3] = borderColor.w;

		samplerDesc.ComparisonFunc = compFunc;

		samplerDesc.MipLODBias = 0.0f;
		samplerDesc.MinLOD = 0.0f;
		samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
		renderer.ThrowIfFailed(renderer.GetDevice()->CreateSamplerState(&samplerDesc, &pSampler));

	}


private:

	Sampler() = default;

	Sampler(Rendering& renderer, D3D11_FILTER filterMode);

	Sampler(Rendering& renderer, D3D11_COMPARISON_FUNC comparisonFunc)
	{
		D3D11_SAMPLER_DESC samplerDesc = {};
		samplerDesc.Filter = D3D11_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT;
		samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
		samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
		samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
		float color[4] = { 1.f,1.f,1.f,1.f };
		samplerDesc.BorderColor[0] = color[0];
		samplerDesc.BorderColor[1] = color[1];
		samplerDesc.BorderColor[2] = color[2];
		samplerDesc.BorderColor[3] = color[3];

		samplerDesc.ComparisonFunc = comparisonFunc;
		
		samplerDesc.MipLODBias = 0.0f;
		samplerDesc.MinLOD = 0.0f;
		samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
		renderer.ThrowIfFailed(renderer.GetDevice()->CreateSamplerState(&samplerDesc, &pSampler));
	}

protected:
	
	Microsoft::WRL::ComPtr<ID3D11SamplerState> pSampler;
};