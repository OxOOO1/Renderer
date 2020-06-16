#pragma once


#pragma once
#include <optional>
#include <filesystem>
#include <sstream>
#include <queue>

#include "ImGUI/imgui.h"
#include "Scene/SceneTree.h"
#include "Components/Drawable/Components/Mesh.h"
#include "Bindable/BindablesH.h"
#include "System/Exception.h"
#include "Model.h"

class Terrain
{

public:

	static constexpr auto MainShaderPath = "cso\\Ground_VS";
	static constexpr auto ShadowMapShaderPath = "cso\\Model_VS(ShadowMap)";


	Terrain(Rendering& renderer);

	void Draw(Rendering& renderer);
	void DrawDepth(Rendering& renderer);



	Geometry mGeometry;

	Shading mShading;

};


