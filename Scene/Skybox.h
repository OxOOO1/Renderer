#pragma once
#include"System/Rendering.h"
#include "Bindable/BindablesH.h"

#include "Components/Drawable/Components/Shading.h"
#include "Components/Drawable/Components/Geometry.h"

class Skybox
{
public:

	Skybox(Rendering& renderer, std::string_view texturePath);

	void Draw(Rendering& renderer)
	{
		mGeometry.SetForShadingPass<1>(renderer);
		mShading.Bind(renderer);

		mShading.mMaterials.back().Bind(renderer);

		mGeometry.ForceDraw(renderer);
	}

	Geometry mGeometry;

	Shading mShading;

};