#pragma once
#include "nlohmann/json.hpp"
#include <unordered_map>
#include <fstream>
#include <array>
#include "ImGUI/imgui.h"

#include "Lights/GlobalLight.h"
#include "Lights/PointLight.h"
#include "Lights/SpotLight.h"

#include "System/Painter.h"
#include "Scene/Controllers/Interactable.h"
#include "Skybox.h"
#include "src/Components/Drawable/Terrain.h"



class Scene
{

	friend class Importer;

public:
	Scene(Rendering& renderer);

	void OnUpdate(Rendering& renderer, GlobalLight& globalLight, Camera& mainCamera);

	void OnEvent(Event& event)
	{
		mInteractables.OnEvent(event);

	}

	void Draw(Rendering& renderer, const DirectX::XMFLOAT3& windPos);

	void DrawDepth(Rendering& renderer, GlobalLight& globalLight, const DirectX::XMFLOAT3& windPos);

public:

	SceneTree octree{ DirectX::XMFLOAT3{0.f, 0.f, 0.f}, DirectX::XMFLOAT3{15000.f,5000.f,15000.f} };

protected:

	std::vector<Model> allStaticModels;
	std::vector<Model> allStaticTransparentModels;
	std::vector<ModelDynamic> allDynamicModels;
	std::vector<ModelLOD> allModelsWithLOD;
	std::vector<ModelSkinned> allSkinnedModels;
	std::vector<ModelInstanced> allInstancedModels;
	//std::vector<TreeInstanced> allInstancedTrees;
	//std::vector<FoliageInstanced> allInstancedFoliage;
	//std::vector<GrassInstanced> allInstancedGrass;

	Skybox skybox;
	Terrain mTerrain;
	 
	Interactables mInteractables;
	
	PointLights allPointLights;
	Spotlights allSpotlights;


};

