#pragma once
#include <optional>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <queue>
#include <algorithm>

#include "ImGUI/imgui.h"

#include "Scene/SceneTree.h"
#include "Bindable/BindablesH.h"
#include "System/Exception.h"

#include "src/Components/Drawable/Components/Geometry.h"
#include "src/Components/Drawable/Components/Shading.h"
#include "src/Components/Drawable/Animation/Transformator.h"
#include "src/Components/Drawable/Animation/Animator.h"
#include "src/Components/Drawable/Components/Instancer.h"

#define PasteDescMethods(type) static ModelType GetType() {return ModelType::##type;}

//TODO Rewrite constructors and V Inherit

enum class ModelType
{
	Model = 0,
	ModelLOD,
	ModelDynamic,
	ModelInstanced,
	ModelSkinned
	
};

//Static model; Mesh Space == World Space;
class Model
{
	friend class Importer;
	friend class Exporter;
public:

	PasteDescMethods(Model)

	static constexpr auto MainShaderPath = "cso\\ModelStatic_VS";
	static constexpr auto ShadowMapShaderPath = "cso\\Model_VS(ShadowMap)";
	static constexpr auto MainPixelShaderPath = "cso\\Model_PS";
	

	Model(Rendering& renderer, std::string_view name) 
		:
		mName(name.data()),
	mShading{ renderer, MainPixelShaderPath },
	mGeometry{ renderer, MainShaderPath, ShadowMapShaderPath,
	{
		{ VertexLayout::Position3D, VertexLayout::Texture2D },
		{ VertexLayout::Normal,VertexLayout::Tangent, VertexLayout::Bitangent }
	} 
	}
	{}

	void OnUpdate(SceneTree& octree);

	void Draw(Rendering& renderer);
	void DrawDepth(Rendering& renderer);

protected:

	Geometry mGeometry;

	Shading mShading;

	std::string mName;

};

//TODO Dynamic OBBox
class ModelDynamic
{
	friend class Importer;
	friend class Exporter;
	friend class Interactables;
public:

	PasteDescMethods(ModelDynamic)

	static constexpr auto MainShaderPath = "cso\\ModelDynamic_VS";
	static constexpr auto ShadowMapShaderPath = "cso\\ModelDynamic(ShadowMap)_VS";


	ModelDynamic(Rendering& renderer, std::string_view name)
		:
		mName(name.data()),
		mShading{ renderer, Model::MainPixelShaderPath },
		mGeometry{ renderer, MainShaderPath, ShadowMapShaderPath,
		{
			{ VertexLayout::Position3DPlusIndex, VertexLayout::Texture2D },
			{ VertexLayout::Normal,VertexLayout::Tangent, VertexLayout::Bitangent }
		}
	}
	{}

	void OnUpdate(Rendering& renderer, SceneTree& octree);

	void Draw(Rendering& renderer);
	void DrawDepth(Rendering& renderer);

protected:

	Transform mTransformator;

	Geometry mGeometry;
	Shading mShading;

	std::string mName;

};



class ModelSkinned
{
	friend class Importer;
	friend class Exporter;
	friend class Interactables;

public:

	PasteDescMethods(ModelSkinned)

	static constexpr auto MainShaderPath = "cso\\ModelSkinned_VS";
	static constexpr auto ShadowMapShaderPath = "cso\\ModelSkinned(ShadowMap)_VS";


	ModelSkinned(Rendering& renderer, std::string_view name)
		:
		mName(name.data()),
		mShading{ renderer, Model::MainPixelShaderPath },
		mGeometry{ renderer, MainShaderPath, ShadowMapShaderPath,
		{
			{ VertexLayout::Position3DPlusIndex, VertexLayout::Texture2D, VertexLayout::BoneIndices, VertexLayout::Weights },
			{ VertexLayout::Normal,VertexLayout::Tangent, VertexLayout::Bitangent }
		}
	}
	{}

	void OnUpdate(Rendering& renderer, SceneTree& octree);

	void Draw(Rendering& renderer);
	void DrawDepth(Rendering& renderer);

	void StartAnimation(int animationIndex = 0)
	{
		mAnimator.StartAnimation(animationIndex);
	}

protected:

	Animator mAnimator;

	Geometry mGeometry;
	Shading mShading;

	std::string mName;

};

class ModelInstanced
{
	friend class Importer;
	friend class Exporter;
public:

	PasteDescMethods(ModelInstanced)

	static constexpr auto MainShaderPath = "cso\\ModelInstanced_VS";
	static constexpr auto ShadowMapShaderPath = "cso\\ModelInstanced(ShadowMap)_VS";

	ModelInstanced(Rendering& renderer, std::string_view name)
		:
		mName(name.data()),
		mShading{ renderer, Model::MainPixelShaderPath },
		mGeometry{ renderer, MainShaderPath, ShadowMapShaderPath,
		{
			{ VertexLayout::Position3D, VertexLayout::Texture2D },
			{ VertexLayout::Normal,VertexLayout::Tangent, VertexLayout::Bitangent }
		}
				}
		{}

	void OnUpdate(Rendering& renderer, SceneTree& octree)
	{
		mInstancer.OnUpdate(renderer, octree);
	}

	void Draw(Rendering& renderer)
	{
		if (mInstancer.numOfInstancesToDraw > 0)
		{
			mInstancer.Bind(renderer);
			mGeometry.DrawShadedInstanced(renderer, mShading, mInstancer.numOfInstancesToDraw);
		}
		
	}
	void DrawDepth(Rendering& renderer)
	{
		if (mInstancer.numOfInstancesToDraw > 0)
		{
			mInstancer.Bind(renderer);
			mGeometry.DrawDepthInstanced(renderer, mShading, mInstancer.numOfInstancesToDraw);
		}
	}

protected:

	Instancer mInstancer;

	Geometry mGeometry;
	Shading mShading;

	std::string mName;

};


class ModelLOD
{
	friend class Importer;
	friend class Exporter;
public:

	PasteDescMethods(ModelLOD)

	void EmplaceBackGeometryLODLevel(Rendering& renderer)
	{
		Geometry g{ renderer, Model::MainShaderPath, Model::ShadowMapShaderPath,
			{
				{ VertexLayout::Position3D, VertexLayout::Texture2D },
				{ VertexLayout::Normal,VertexLayout::Tangent, VertexLayout::Bitangent }
			}
		};

		mGeometryLOD.push_back(std::move(g));
	}

	ModelLOD(Rendering& renderer, std::string_view name)
		:
		mName(name.data()),
		mShading{ renderer, Model::MainPixelShaderPath }
	{}

	void OnUpdate(SceneTree& octree);

	void Draw(Rendering& renderer);
	void DrawDepth(Rendering& renderer);

private:

	std::vector<Geometry> mGeometryLOD;

	Shading mShading;

	UINT indexToMainBBox;
	UINT indexToActiveLODNode = 0;

	std::string mName;
};



