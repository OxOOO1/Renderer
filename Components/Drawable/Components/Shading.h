#pragma once

#include "Material.h"

class Shading
{

public:

	Shading(Rendering& renderer, const std::string& pixelShaderPath)
	{
		pPixelShader = PixelShader::Resolve(renderer, pixelShaderPath);
	}

	void Bind(Rendering& renderer)
	{
		Sampler::Get(renderer).Bind(renderer, 0u);
		Sampler::GetComparisonSampler(renderer).Bind(renderer, 1u);
		pPixelShader->Bind(renderer);
	}

	std::vector<Material> mMaterials;
	std::shared_ptr<PixelShader> pPixelShader;

};