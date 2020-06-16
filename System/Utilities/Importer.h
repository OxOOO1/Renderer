#pragma once

#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"

#include "src/Components/Drawable/Components/Geometry.h"
#include "src/Components/Drawable/Components/Shading.h"
#include "src/Components/Drawable/Components/Instancer.h"

#include "src/Scene/Scene.h"

//TODO Reduce number of allocations caused by std::vecs

constexpr auto MainFilePath = "assets\\models\\";

class Importer
{

public:

	static Importer& Get()
	{
		static Importer sImporter;
		return sImporter;
	}

	void ImportModel(Rendering& renderer, Scene& scene, const std::string& modelName);

	void ImportModelAssimp(Rendering& renderer, Scene& scene, const std::string& filePath, ModelType type, bool transparent = false);

	void ImportInstancedModelAssimp(Rendering& renderer, Scene& scene, const std::string& modelPath, const std::string& instancesPath);

	void ParseInstancesAssimp(Rendering& renderer, aiNode* pRoot, Instancer& instancer, SceneTree& octree, UINT indexInOctree);

	template<typename T>
	static void ReadVecFromFile(FileIO::ReadFromFile& file, std::vector<T>& vec)
	{
		size_t size = 0;
		file.Read(&size);
		vec.resize(size);
		file.Read(vec.data(), size * sizeof(T));
	}

	void ImportGeometryAssimp(Rendering& renderer, Geometry& to, std::string_view fileName)
	{
		AssimpImporter importer;
		auto pScene = importer.ReadModelAssimp(fileName);
		ParseModelGeometryAssimp(renderer, pScene->mNumMeshes, pScene->mMeshes, to, nullptr);
	}

private:

	void ImportGeometry(Rendering& renderer, Scene& scene, Geometry& mGeometry, FileIO::ReadFromFile& file);
	bool ImportMaterials(Rendering& renderer, Shading& shading, nlohmann::json& meta);

	void ParseModelGeometryAssimp(Rendering& renderer, UINT numOfMeshes, aiMesh** mMeshes, Geometry& mGeometry, SceneTree* octree, Transform* animator = nullptr, bool importBones = false);
	//Textures should reside in the same folder with the model
	void ParseModelMaterialsAssimp(Rendering& renderer, const aiScene* const pScene, const std::filesystem::path& _path, Shading& shader, bool transparent);
	void ParseModelTransformNodesAssimp(Rendering& renderer, Transform* animator, const aiNode* pRootNode);
	void ParseModelAnimationAssimp(const aiScene* pScene, Animator& animator);

	void FillRawVertexBuffer(const aiMesh* mesh, Geometry& mGeometry, std::vector<struct BoneVertex>& boneVerts, float indexToCBufferMatrix = -1);
	void FillRawIndexBuffer(const aiMesh* mesh, Geometry& mGeometry);

private:
	
	Importer() = default;

	struct AssimpImporter
	{
		const aiScene* ReadModelAssimp(std::string_view file, bool separatePivotNodes = false, bool readMaterials = true, bool readAnimations = false, int PostProcessOps = PostProcessOpsDefault)
		{
			assimport.SetPropertyBool(AI_CONFIG_IMPORT_FBX_PRESERVE_PIVOTS, separatePivotNodes);
			assimport.SetPropertyBool(AI_CONFIG_IMPORT_FBX_READ_MATERIALS, readMaterials);
			assimport.SetPropertyBool(AI_CONFIG_IMPORT_FBX_READ_ANIMATIONS, readAnimations);
			const auto pScene = assimport.ReadFile(std::string{ file }, PostProcessOps);
			assert(pScene && "Cant Import Model");
			return pScene;

		}
		Assimp::Importer assimport;


		static constexpr auto PostProcessOpsDefault = aiProcess_ConvertToLeftHanded | aiProcess_Triangulate | aiProcess_GenNormals | aiProcess_CalcTangentSpace | aiProcess_GenUVCoords;
		static constexpr auto PostProcessSkinnedDefault = aiProcess_LimitBoneWeights | aiProcess_SplitByBoneCount | aiProcess_ConvertToLeftHanded | aiProcess_Triangulate | aiProcess_JoinIdenticalVertices | aiProcess_GenNormals | aiProcess_CalcTangentSpace | aiProcess_GenUVCoords;

	};

};
