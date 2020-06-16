#include "Material.h"

void Material::Create(Rendering & renderer, bool transparenceBlending, bool alphaToCoverage)
{
	constPSDescCBuffer.Create(renderer, 2u, mDesc);
	mBlenderState = &Blender::Get(renderer, transparenceBlending, alphaToCoverage);
	mRasterizerState = &Rasterizer::Get(renderer, mDesc.hasActiveAlpha);
	mDesc.transparent = transparenceBlending;
}

void Material::Bind(Rendering & renderer)
{
	for (auto& t : mTextures) {
		t->Bind(renderer);
	}
	for (auto& t : mTextureArrays) {
		t->Bind(renderer);
	}

	constPSDescCBuffer.Bind(renderer);

	mBlenderState->Bind(renderer);
	mRasterizerState->Bind(renderer);
}

void Material::BindForDepthPass(Rendering & renderer)
{
	if (mDesc.hasActiveAlpha)
	{
		Sampler::Get(renderer).Bind(renderer);
		assert(!mTextures.empty());
		mTextures.at(0)->Bind(renderer);
		pPixelShaderDepthPass->Bind(renderer);
		mBlenderState->Bind(renderer);
		Rasterizer::GetWithDepthBias(renderer, true).Bind(renderer);
	}
	else
	{
		Blender::Get(renderer).Bind(renderer);
		PixelShader::SetNull(renderer);
		Rasterizer::GetWithDepthBias(renderer).Bind(renderer);
	}
}
