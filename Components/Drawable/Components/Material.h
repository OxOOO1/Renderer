#pragma once

#include "Bindable/BindablesH.h"
#include "System/Rendering.h"

class Material
{
public:

	void Create(Rendering& renderer, bool transparenceBlending = false, bool alphaToCoverage = false);

	void Bind(Rendering& renderer);

	void BindForDepthPass(Rendering& renderer);

	void AddTexture(std::shared_ptr<Texture>&& t)
	{
		mTextures.push_back(t);
	}
	void AddTexture(std::shared_ptr<TextureArray>&& t)
	{
		mTextureArrays.push_back(t);
	}

	struct Description
	{
		int32_t hasDiffuse = FALSE;
		int32_t hasNormal = FALSE;
		int32_t hasSpecular = FALSE;
		int32_t hasActiveAlpha = FALSE;
		int32_t isEmitter = FALSE;
		float emitIntensity = 1.0f;
		int32_t transparent = FALSE;
		float pad2;
	}mDesc;

	ConstPixelCBuffer<Description> constPSDescCBuffer;

	std::shared_ptr<PixelShader> pPixelShaderDepthPass;

	std::vector<std::shared_ptr<Texture>> mTextures;
	std::vector<std::shared_ptr<TextureArray>> mTextureArrays;

	Blender* mBlenderState;
	Rasterizer* mRasterizerState;

	std::vector<unsigned short> indicesToMeshes;

	//For export
	struct FileNames
	{
		std::string diffuse;
		std::string normal;
		std::string specular;
		std::string emissive;
	}mFileNames;

	std::string mName;
};

class TreeMaterial : public Material
{
public:

	bool isLeaves = false;
	bool isBillboard = false;
	
	std::shared_ptr<Texture> leavesWeightTexture;

};