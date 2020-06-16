#include "Scene.h"
#include "src/System/Utilities/Importer.h"
#include "src/System/Utilities/Exporter.h"

Scene::Scene(Rendering& renderer)
	:
	skybox{ renderer, "DarkStorm4K.dds" }
	,mTerrain{ renderer }
	,mInteractables{renderer}
{
	//Import model as basic static mesh
	//Importer::Get().ImportModelAssimp(renderer, *this, "assets\\models\\Knight.fbx", ModelType::Model);

	//Export model as basic static mesh
	//Exporter::Export(allStaticModels.front(), "Knight");

	//Import custom model from asset files
	//Importer::Get().ImportModel(renderer, *this, "Knight");

	//Import movable dynamic mesh
	//Importer::Get().ImportModelAssimp(renderer, *this, "assets\\models\\Knight.fbx", ModelType::ModelDynamic);

	//Import Animated Skinned Mesh (Armature-Only)
	Importer::Get().ImportModelAssimp(renderer, *this, "assets\\models\\Knight.fbx", ModelType::ModelSkinned);
	//Add skinned mesh as interactable
	mInteractables.Add(&allSkinnedModels.back());
	

	Importer::Get().ImportGeometryAssimp(renderer, mTerrain.mGeometry, "assets\\models\\Terrain.fbx");
	Importer::Get().ImportGeometryAssimp(renderer, skybox.mGeometry, "assets\\models\\skyboxes\\SkySphere.fbx");

	
}

void Scene::OnUpdate(Rendering & renderer, GlobalLight & globalLight, Camera& mainCamera)
{
	allSpotlights.OnUpdate(renderer);
	allPointLights.OnUpdate(renderer);

	for (auto& model : allStaticModels)
	{
		model.OnUpdate(octree);
	}
	for (auto& tModel : allStaticTransparentModels)
	{
		tModel.OnUpdate(octree);
	}
	for (auto& ml : allModelsWithLOD)
	{
		ml.OnUpdate(octree);
	}
	for (auto& m : allDynamicModels)
	{
		m.OnUpdate(renderer, octree);
	}
	for (auto& model : allInstancedModels)
	{
		model.OnUpdate(renderer, octree);
	}
	for (auto& m : allSkinnedModels)
	{
		m.OnUpdate(renderer, octree);
	}

	//TreeInstanced::CalculateBillboardLightIntensity(globalLight);
	/*for (auto& tree : allInstancedTrees)
	{
		tree.OnUpdate(renderer, octree);
	}*/
	/*for (auto& fol : allInstancedFoliage)
	{
		fol.OnUpdate(renderer, octree);
	}*/
	/*for (auto& grass : allInstancedGrass)
	{
		grass.OnUpdate(renderer, octree);
	}*/
	

	mInteractables.OnUpdate(renderer, mainCamera, octree);

}

void Scene::Draw(Rendering & renderer, const DirectX::XMFLOAT3 & windPos)
{

	for (auto& model : allStaticModels)
	{
		model.Draw(renderer);
	}
	for (auto& ml : allModelsWithLOD)
	{
		ml.Draw(renderer);
	}
	for (auto& m : allDynamicModels)
	{
		m.Draw(renderer);
	}
	for (auto& m : allSkinnedModels)
	{
		m.Draw(renderer);
	}
	for (auto& model : allInstancedModels)
	{
		model.Draw(renderer);
	}
	
	/*for (auto& tree : allInstancedTrees)
	{
		tree.Draw(renderer, windPos);
	}*/
	/*for (auto& fol : allInstancedFoliage)
	{
		fol.Draw(renderer, windPos);
	}
	for (auto& grass : allInstancedGrass)
	{
		grass.Draw(renderer, windPos, octree);
	}
	
	/*for (auto& inter : allInteractables.allMeshes)
	{
		inter.Draw(renderer);
	}*/

	mTerrain.Draw(renderer);

	RenderTarget::SetDefaultShaderResourceForPS();

	skybox.Draw(renderer);
	for (auto& tModel : allStaticTransparentModels)
	{
		tModel.Draw(renderer);
	}

	//allPointLights.Draw(renderer);
	//allPointLights.ShowControlWindow(renderer);

	//allSpotlights.Draw(renderer);
	//allSpotlights.ShowControlWindow(renderer);

	mInteractables.DrawUI(renderer);

}

void Scene::DrawDepth(Rendering & renderer, GlobalLight & globalLight, const DirectX::XMFLOAT3 & windPos)
{
	
	//mTerrain.DrawDepth(renderer);

	for (auto& model : allStaticModels)
	{
		model.DrawDepth(renderer);
	}
	for (auto& ml : allModelsWithLOD)
	{
		ml.DrawDepth(renderer);
	}
	for (auto& m : allDynamicModels)
	{
		m.DrawDepth(renderer);
	}
	for (auto& m : allSkinnedModels)
	{
		m.DrawDepth(renderer);
	}
	for (auto& model : allInstancedModels)
	{
		model.DrawDepth(renderer);
	}
	/*
	for (auto& tree : allInstancedTrees)
	{
		tree.DrawDepth(renderer, windPos);
	}
	for (auto& fol : allInstancedFoliage)
	{
		fol.DrawDepth(renderer, windPos);
	}*/
	/*for (auto& grass : allInstancedGrass)
	{
	grass.draw(renderer, octree);
	}*/
	
	
}

