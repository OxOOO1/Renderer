#include "Terrain.h"

Terrain::Terrain(Rendering& renderer)
	:
	mShading{ renderer, "cso\\Ground_PS" },
	mGeometry{ renderer, MainShaderPath, ShadowMapShaderPath,
	{
		{ VertexLayout::Position3D, VertexLayout::Texture2D },
		{ VertexLayout::Normal,VertexLayout::Tangent, VertexLayout::Bitangent }
	}
}
{

	std::shared_ptr<TextureArray> tArr = TextureArray::Resolve(
		renderer,
		{
	"assets\\textures\\terrain\\T_ground_meadow_grass_01_A_SM.dds",
	"assets\\textures\\terrain\\T_forest_ground_needles_01_A_SM.dds"
		},
		0u);
	std::shared_ptr<TextureArray> tArr2 = TextureArray::Resolve(
		renderer,
		{
	"assets\\textures\\terrain\\T_ground_meadow_grass_01_N.dds",
	"assets\\textures\\terrain\\T_forest_ground_needles_01_N.dds"
		},
		1u);

	Material mat;
	mat.AddTexture(std::move(tArr));
	mat.AddTexture(std::move(tArr2));
	mat.Create(renderer);
	mShading.mMaterials.push_back(std::move(mat));
}

void Terrain::Draw(Rendering & renderer)
{
	mGeometry.SetForShadingPass(renderer);
	mShading.Bind(renderer);

	mShading.mMaterials.back().Bind(renderer);

	mGeometry.ForceDraw(renderer);
}

void Terrain::DrawDepth(Rendering & renderer)
{
	mGeometry.SetForDepthPass(renderer);

	mShading.mMaterials.back().BindForDepthPass(renderer);

	mGeometry.ForceDraw(renderer);
}
