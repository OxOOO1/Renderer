#include "Importer.h"

#include "System/Utilities/MathConverter.h"

/////////////////////////////////////////////////////////
//
//       CUSTOM FILE IMPORT
//
/////////////////////////////////////////////////////////

void Importer::ImportModel(Rendering & renderer, Scene & scene, const std::string & fileName)
{
	FileIO::ReadFromFile fileAsset((MainFilePath + fileName + ".asset").c_str());
	assert(fileAsset.IsOpen() && "Missing asset file");

	std::ifstream fileMeta((MainFilePath + fileName + ".meta"));
	assert(fileMeta.is_open() && "Missing material file");
	nlohmann::json jj = nlohmann::json::parse(fileMeta);

	ModelType type = jj["ModelTypeID"];

	switch (type)
	{
	case ModelType::Model:
	{

		Model model{ renderer, fileName };

		model.mName = jj["ModelName"];

		ImportGeometry(renderer, scene, model.mGeometry, fileAsset);

		bool isTransparent = ImportMaterials(renderer, model.mShading, jj);

		if (isTransparent) {
			scene.allStaticTransparentModels.push_back(std::move(model));
		}
		else {
			scene.allStaticModels.push_back(std::move(model));
		}

		break;
	}

	case ModelType::ModelDynamic:
		break;
	case ModelType::ModelInstanced:
		break;
	case ModelType::ModelSkinned:
		break;
	default:
		break;
	}

}

void Importer::ImportGeometry(Rendering& renderer, Scene& scene,Geometry & mGeometry, FileIO::ReadFromFile & file)
{
	ReadVecFromFile(file, mGeometry.mMeshes);
	ReadVecFromFile(file, mGeometry.mRawVertices);
	ReadVecFromFile(file, mGeometry.mRawIndices);

	for (auto& buffer : mGeometry.mRawVertexBuffers)
	{
		buffer.ReadFromFile(file);
	}

	for (auto& mesh : mGeometry.mMeshes)
	{
		{
			AxisAlignedBoundingBox box;
			box.Init(mesh.numOfVertices, &mGeometry.mRawVertices.at(mesh.firstVertexPos), sizeof(DirectX::XMFLOAT3));
			mesh.indexToBBox = scene.octree.InsertBox(std::move(box));
		}
	}

	mGeometry.FillGPUBuffersWithRawBuffers(renderer);

	if (mGeometry.mMeshes.size() > 1)
	{
		AxisAlignedBoundingBox box;
		box.Init(mGeometry.mRawVertices.size(), (mGeometry.mRawVertices.data()), sizeof(DirectX::XMFLOAT3));
		mGeometry.indexToBBox = scene.octree.InsertBox(std::move(box));
	}
	else mGeometry.indexToBBox = mGeometry.mMeshes.back().indexToBBox;

}

//TODO Sumbmit tex names to queue and load in sep thread
bool Importer::ImportMaterials(Rendering& renderer, Shading & shading, nlohmann::json & meta)
{
	bool isTransparent = false;
	size_t numOfMaterials = meta["NumOfMaterials"];
	shading.mMaterials.resize(numOfMaterials);

	for (size_t i = 0; i < numOfMaterials; i++)
	{
		auto& mat = shading.mMaterials[i];

		mat.mName = meta[std::to_string(i)]["Name"];

		auto it = meta[mat.mName].find("Diffuse");
		if (it != meta[mat.mName].end()){
			mat.mDesc.hasDiffuse = true;
			std::string name = it.value();
			mat.AddTexture(Texture::Resolve(renderer,MainFilePath + name, Texture::DiffuseSlot));
			mat.mFileNames.diffuse = name;
		}
		it = meta[mat.mName].find("Normal");
		if (it != meta[mat.mName].end()){
			mat.mDesc.hasNormal = true;
			std::string name = it.value();
			mat.AddTexture(Texture::Resolve(renderer, MainFilePath + name, Texture::NormalSlot));
			mat.mFileNames.normal = name;
		}
		it = meta[mat.mName].find("Specular");
		if ( it != meta[mat.mName].end()){
			mat.mDesc.hasSpecular = true;
			std::string name = it.value();
			mat.AddTexture(Texture::Resolve(renderer, MainFilePath + name, Texture::SpecularSlot));
			mat.mFileNames.specular = name;
		}
		it = meta[mat.mName].find("Emission");
		if ( it != meta[mat.mName].end()){
			mat.mDesc.isEmitter = true;
			std::string name = it.value();
			mat.AddTexture(Texture::Resolve(renderer, MainFilePath + name, Texture::EmissionSlot));
			mat.mFileNames.emissive = name;
		}

		mat.mDesc.hasActiveAlpha = meta[mat.mName]["HasAlpha"];
		if (mat.mDesc.hasActiveAlpha)
			mat.pPixelShaderDepthPass = PixelShader::Resolve(renderer, "cso\\Model_PS(DepthOnly)");
		mat.mDesc.transparent = meta[mat.mName]["Transparent"];
		if(mat.mDesc.transparent)
			isTransparent = true;

		std::vector<unsigned short> t = meta[mat.mName]["IndicesToMeshes"];
		mat.indicesToMeshes = std::move(t);

		mat.Create(renderer, mat.mDesc.transparent);

	}

	return isTransparent;

}

/////////////////////////////////////////////////////////
//
//       ASSIMP IMPORT
//
/////////////////////////////////////////////////////////

void Importer::ImportModelAssimp(Rendering & renderer, Scene & scene, const std::string & fileName, ModelType type, bool transparent)
{
	AssimpImporter importer;

	switch (type)
	{
	case ModelType::Model:
	{
		Model model(renderer, fileName);

		auto pScene = importer.ReadModelAssimp(fileName);

		ParseModelGeometryAssimp(renderer, pScene->mNumMeshes, pScene->mMeshes, model.mGeometry, &scene.octree);
		ParseModelMaterialsAssimp(renderer, pScene, fileName, model.mShading, transparent);

		if (transparent) {
			scene.allStaticTransparentModels.push_back(std::move(model));
		}else {
			scene.allStaticModels.push_back(std::move(model));
		}

		break;
	}
	case ModelType::ModelLOD:
	{
		ModelLOD model(renderer, fileName);

		auto pScene = importer.ReadModelAssimp(fileName);

		//Find LOD Head Node
		int count = 0;
		auto rootNode = pScene->mRootNode;
		while (rootNode->mNumChildren < 2){
			rootNode = rootNode->mChildren[0];
			count++;
			assert(count < 10 && "No LOD child nodes found");
		}

		for (int i = 0; i < rootNode->mNumChildren; i++)
		{
			auto lodNode = rootNode->mChildren[i];
			//Copy meshes owned by this node
			std::unique_ptr<aiMesh*[]> lodMeshes = std::make_unique<aiMesh*[]>(lodNode->mNumMeshes);
			for (int i = 0; i < lodNode->mNumMeshes; i++){
				lodMeshes[i] = pScene->mMeshes[lodNode->mMeshes[i]];
			}
			
			model.EmplaceBackGeometryLODLevel(renderer);
			ParseModelGeometryAssimp(renderer, lodNode->mNumMeshes, lodMeshes.get(), model.mGeometryLOD.back(), &scene.octree);
		}

		model.indexToMainBBox = model.mGeometryLOD.front().indexToBBox;
		ParseModelMaterialsAssimp(renderer, pScene, fileName, model.mShading, transparent);
		scene.allModelsWithLOD.push_back(std::move(model));

		break;
	}
	case ModelType::ModelDynamic:
	{
		ModelDynamic model(renderer, fileName);

		auto pScene = importer.ReadModelAssimp(fileName);

		ParseModelTransformNodesAssimp(renderer, &model.mTransformator, pScene->mRootNode);
		model.mTransformator.Init(renderer);
		ParseModelGeometryAssimp(renderer, pScene->mNumMeshes, pScene->mMeshes, model.mGeometry, nullptr, &model.mTransformator);
		ParseModelMaterialsAssimp(renderer, pScene, fileName, model.mShading, transparent);

		scene.allDynamicModels.push_back(std::move(model));

		break;
	}
	case ModelType::ModelSkinned:
	{
		ModelSkinned model(renderer, fileName);
		auto pScene = importer.ReadModelAssimp(fileName, true, true, true, AssimpImporter::PostProcessSkinnedDefault);;

		ParseModelTransformNodesAssimp(renderer, &model.mAnimator, pScene->mRootNode);
		ParseModelAnimationAssimp(pScene, model.mAnimator);
		model.mAnimator.Init(renderer);
		ParseModelGeometryAssimp(renderer, pScene->mNumMeshes, pScene->mMeshes, model.mGeometry, &scene.octree, &model.mAnimator, true);
		ParseModelMaterialsAssimp(renderer, pScene, fileName, model.mShading, transparent);

		scene.allSkinnedModels.push_back(std::move(model));
		
	}
		break;
	default:
		assert(false && "Unrecognized Model Type");
		break;
	}
}

void ParseNodeAnimationChannel(const aiNodeAnim* chIn, AnimationClip::Channel& chOut)
{
	chOut.nodeName = chIn->mNodeName.C_Str();

	chOut.positions.resize(chIn->mNumPositionKeys);
	static_assert(sizeof(AnimationClip::Channel::Position) == sizeof(aiVectorKey));
	std::memcpy(chOut.positions.data(), chIn->mPositionKeys, chIn->mNumPositionKeys * sizeof(AnimationClip::Channel::Position));
	
	chOut.orientations.resize(chIn->mNumRotationKeys);
	for (int i = 0; i < chIn->mNumRotationKeys; i++) {
		auto& rot = chIn->mRotationKeys[i].mValue;
		chOut.orientations.at(i).value = DirectX::XMVectorSet(rot.x, rot.y, rot.z, rot.w);
		chOut.orientations.at(i).time = chIn->mRotationKeys[i].mTime;
	}

	chOut.scalings.resize(chIn->mNumScalingKeys);
	static_assert(sizeof(AnimationClip::Channel::Scale) == sizeof(aiVectorKey));
	std::memcpy(chOut.scalings.data(), chIn->mScalingKeys, chIn->mNumScalingKeys * sizeof(AnimationClip::Channel::Scale));

}

void ParseAnimationClip(const aiAnimation* animIn, AnimationClip& animOut)
{
	if (animIn->mTicksPerSecond == 0)
		animOut.ticksPerSec = 25;
	else animOut.ticksPerSec = animIn->mTicksPerSecond;

	animOut.name = animIn->mName.C_Str();
	animOut.durationInTicks = animIn->mDuration;

	animOut.mChannels.reserve(animIn->mNumChannels);
	
	for (int i = 0; i < animIn->mNumChannels; i++)
	{
		AnimationClip::Channel ch;
		ParseNodeAnimationChannel(animIn->mChannels[i], ch);
		animOut.mChannels.push_back(std::move(ch));
	}
}

void Importer::ParseModelAnimationAssimp(const aiScene * pScene, Animator & animator)
{
	animator.mAnimations.reserve(pScene->mNumAnimations);

	for (int i = 0; i < pScene->mNumAnimations; i++)
	{
		AnimationClip anim;
		ParseAnimationClip(pScene->mAnimations[i], anim);
		animator.mAnimationNameToIndexMap[anim.name] = animator.mAnimations.size();
		animator.mAnimations.push_back(std::move(anim));
	}

}



void GenerateBBoxForGeometry(Geometry& mGeometry, SceneTree& octree)
{
	AxisAlignedBoundingBox box;
	box.Init(mGeometry.mRawVertices.size(), (mGeometry.mRawVertices.data()), sizeof(DirectX::XMFLOAT3));
	mGeometry.indexToBBox = octree.InsertBox(std::move(box));
}

void Importer::ImportInstancedModelAssimp(Rendering & renderer, Scene & scene, const std::string & modelPath, const std::string & instancesPath)
{
	AssimpImporter importer;

	ModelInstanced model{ renderer, modelPath };

	auto pScene = importer.ReadModelAssimp(modelPath);

	ParseModelGeometryAssimp(renderer, pScene->mNumMeshes, pScene->mMeshes, model.mGeometry, &scene.octree);
	ParseModelMaterialsAssimp(renderer, pScene, modelPath, model.mShading, false);

	GenerateBBoxForGeometry(model.mGeometry, scene.octree);

	auto pInstances = importer.ReadModelAssimp(instancesPath);

	ParseInstancesAssimp(renderer, pInstances->mRootNode, model.mInstancer, scene.octree, model.mGeometry.indexToBBox);

}

void ParseInstanceNode(Instancer& instancer, aiNode* node, SceneTree& octree, UINT mainBoxindexInOctree)
{
	if (!node->mTransformation.IsIdentity())
	{
		auto newBox = octree.allModelsAABBoxes[mainBoxindexInOctree];

		DirectX::XMMATRIX m = MathConverter::AssimpToDirectX(node->mTransformation);

		newBox.dAABB.Transform(newBox.dAABB, /*DirectX::XMMatrixTranspose(*/m);
		instancer.indicesToBBoxes.push_back(octree.InsertBox(std::move(newBox)));

		instancer.transformMatrices.push_back(m);

	}

	for (size_t i = 0; i < node->mNumChildren; i++)
	{
		ParseInstanceNode(instancer, node->mChildren[i], octree, mainBoxindexInOctree);
	}
}

void Importer::ParseInstancesAssimp(Rendering & renderer, aiNode * pRoot, Instancer & instancer, SceneTree & octree, UINT mainBoxindexInOctree)
{
	ParseInstanceNode(instancer, pRoot, octree, mainBoxindexInOctree);
	instancer.Init(renderer);
}

void ParseNode(Transform* animator, const aiNode* node, int parentNodeIndex = -1)
{

	if (node->mName.length != 0)
	{
		UINT16 id = animator->mNodes.size();

		if (parentNodeIndex != -1)
			animator->mNodes.at(parentNodeIndex).indicesToChildren.push_back(id);

		Node nodeOut;
		nodeOut.name = node->mName.C_Str();
		nodeOut.index = id;
		nodeOut.indexToParent = parentNodeIndex;

		parentNodeIndex = id;

		auto m = node->mTransformation;
		nodeOut.matParentSpace = MathConverter::AssimpToDirectX(m);

		animator->mNodeNameToNodeIDMap[nodeOut.name] = id;

		if (node->mNumMeshes > 0){
			for (int i = 0; i < node->mNumMeshes; i++){
				nodeOut.indicesToMeshes.push_back(node->mMeshes[i]);
			}
		}

		animator->mNodes.push_back(std::move(nodeOut));

	}
	else
	{
		for (size_t i = 0; i < node->mNumChildren; i++)
		{
			node->mChildren[i]->mTransformation = node->mTransformation;
		}
	}

	for (size_t i = 0; i < node->mNumChildren; i++)
	{
		ParseNode(animator, node->mChildren[i], parentNodeIndex);
	}

}

void Importer::ParseModelTransformNodesAssimp(Rendering & renderer, Transform* animator, const aiNode* node)
{
	ParseNode(animator, node);
}

struct BoneVertex
{
	std::array<UINT8, 4> indicesToBones{ 0 };
	std::array<float, 4> weights{ 0.f };
	int counter = 0;
};

void FillBoneVertices(std::vector<BoneVertex>& boneVerts, const aiMesh* mesh, Transform* animator)
{
	boneVerts.resize(mesh->mNumVertices);
	for (unsigned int a = 0; a < mesh->mNumBones; a++)
	{
		const aiBone *bone = mesh->mBones[a];

		auto it = animator->mNodeNameToNodeIDMap.find(bone->mName.C_Str());
		assert(it != animator->mNodeNameToNodeIDMap.end());
		auto boneId = it->second;

		animator->mNodes[boneId].matInvBindPose = MathConverter::AssimpToDirectX(bone->mOffsetMatrix);
		animator->mNodes.at(boneId).isJoint = true;

		for (unsigned int b = 0; b < bone->mNumWeights; b++)
		{
			auto& vertex = boneVerts[bone->mWeights[b].mVertexId];

			vertex.indicesToBones[vertex.counter] = boneId;
			vertex.weights[vertex.counter] = bone->mWeights[b].mWeight;
			vertex.counter++;
			assert(vertex.counter <= 4);

		}
	}
}

void Importer::ParseModelGeometryAssimp(Rendering & renderer, UINT numOfMeshes, aiMesh** mMeshes, Geometry & mGeometry, SceneTree* octree, Transform* animator, bool parseBones)
{
	UINT firstVertexPos = 0;
	UINT firstIndexPos = 0;

	//Collect indices to nodes to emplace them into vertices
	std::vector<int> indicesToNodes;
	if (animator)
	{
		indicesToNodes.resize(numOfMeshes);
		for (int i = 0; i < animator->mNodes.size(); i++){
			auto& node = animator->mNodes[i];
			for (int m = 0; m < node.indicesToMeshes.size(); m++){
				auto indexToMesh = node.indicesToMeshes[m];
				indicesToNodes[indexToMesh] = node.index;
			}
		}
	}

	//All meshes will be combined into single VB/IB;
	for (size_t i = 0; i < numOfMeshes; i++)
	{
		auto mesh = mMeshes[i];

		float indexToNode = -1;

		std::vector<BoneVertex> boneVerts;
		if (animator){
			indexToNode = indicesToNodes[i];
			if (parseBones){
				FillBoneVertices(boneVerts, mesh, animator);
			}
		}
		
		FillRawVertexBuffer(mesh, mGeometry, boneVerts, indexToNode);
		FillRawIndexBuffer(mesh, mGeometry);

		UINT IndexCount = mesh->mNumFaces * 3;

		mGeometry.mMeshes.emplace_back(renderer, firstIndexPos, firstVertexPos, IndexCount);
		mGeometry.mMeshes.back().indexToMaterial = mesh->mMaterialIndex;
		mGeometry.mMeshes.back().numOfVertices = mesh->mNumVertices;
		if (octree)
		{
			AxisAlignedBoundingBox box;
			box.Init(mesh->mNumVertices, reinterpret_cast<DirectX::XMFLOAT3*>(mesh->mVertices), sizeof(DirectX::XMFLOAT3));
			DirectX::XMVECTOR ext = DirectX::XMVector3ClampLength(DirectX::XMLoadFloat3(&box.dAABB.Extents), 25.f, 5000.f);
			DirectX::XMStoreFloat(&mGeometry.mMeshes.back().cullDistance, DirectX::XMVectorScale(DirectX::XMVector3Length(ext), 100.f));

			mGeometry.mMeshes.back().indexToBBox = octree->InsertBox(std::move(box));
		}

		firstVertexPos += mesh->mNumVertices;
		firstIndexPos += IndexCount;

	}

	mGeometry.FillGPUBuffersWithRawBuffers(renderer);

	if (octree)
	{
		if (mGeometry.mMeshes.size() > 1)
		{
			AxisAlignedBoundingBox box;
			box.Init(mGeometry.mRawVertices.size(), (mGeometry.mRawVertices.data()), sizeof(DirectX::XMFLOAT3));
			mGeometry.indexToBBox = octree->InsertBox(std::move(box));
		}
		else mGeometry.indexToBBox = mGeometry.mMeshes.back().indexToBBox;
	}
	
}


void Importer::ParseModelMaterialsAssimp(Rendering & renderer, const aiScene * const pScene, const std::filesystem::path & _path, Shading & shader, bool transparent)
{
	shader.mMaterials.resize(pScene->mNumMaterials);

	for (int i = 0; i < pScene->mNumMaterials; i++)
	{
		auto& materialIn = *pScene->mMaterials[i];
		auto& materialOut = shader.mMaterials[i];

		materialOut.mName = materialIn.GetName().C_Str();

		using namespace std::string_literals;
		aiString texFileName;

		const auto path = _path.parent_path().string() + "\\";

		if (materialIn.GetTexture(aiTextureType_DIFFUSE, 0, &texFileName) == aiReturn_SUCCESS)
		{
			materialOut.mDesc.hasDiffuse = TRUE;

			texFileName = aiScene::GetShortFilename(texFileName.C_Str());
			std::string name = texFileName.C_Str();
			name = { name.begin(), name.end() - 4 };

			std::string finalPath;

			finalPath = path + name.data();

			std::ifstream f(finalPath + ".dds");
			if (f.good()) {
				finalPath = finalPath + ".dds";
				name = name + ".dds";
			}
			else {
				finalPath = finalPath + ".png";
				name = name + ".png";
			}

			auto pTexture = Texture::Resolve(renderer, finalPath.c_str(), Texture::DiffuseSlot);
			materialOut.mDesc.hasActiveAlpha = pTexture->HasActiveAlpha();
			materialOut.AddTexture(std::move(pTexture));
			materialOut.mFileNames.diffuse = name;
		}

		if (materialIn.GetTexture(aiTextureType_HEIGHT, 0, &texFileName) == aiReturn_SUCCESS || materialIn.GetTexture(aiTextureType_NORMALS, 0, &texFileName) == aiReturn_SUCCESS)
		{
			texFileName = aiScene::GetShortFilename(texFileName.C_Str());
			std::string name = texFileName.C_Str();
			if (name.length() > 4)
			{
				name = { name.begin(), name.end() - 4 };
				name = name + ".dds";
				materialOut.AddTexture(Texture::Resolve(renderer, (path + name).c_str(), Texture::NormalSlot));
				materialOut.mDesc.hasNormal = TRUE;
				materialOut.mFileNames.normal = name;
			}
		}

		if (materialIn.GetTexture(aiTextureType_SPECULAR, 0, &texFileName) == aiReturn_SUCCESS)
		{
			texFileName = aiScene::GetShortFilename(texFileName.C_Str());
			std::string name = texFileName.C_Str();
			name = { name.begin(), name.end() - 4 };
			name = name + ".dds";
			materialOut.AddTexture(Texture::Resolve(renderer, (path + name).c_str(), Texture::SpecularSlot));
			materialOut.mDesc.hasSpecular = TRUE;
			materialOut.mFileNames.specular = name;
		}

		if (materialIn.GetTexture(aiTextureType_EMISSIVE, 0, &texFileName) == aiReturn_SUCCESS || materialIn.GetTexture(aiTextureType_AMBIENT, 0, &texFileName) == aiReturn_SUCCESS)
		{
			texFileName = aiScene::GetShortFilename(texFileName.C_Str());
			std::string name = texFileName.C_Str();
			if (name.length() > 4)
			{
				name = { name.begin(), name.end() - 4 };
				name = name + ".dds";
				materialOut.AddTexture(Texture::Resolve(renderer, (path + name).c_str(), Texture::EmissionSlot));
				materialOut.mDesc.isEmitter = TRUE;
				materialOut.mFileNames.emissive = name;
			}
		}

		if (materialOut.mDesc.hasActiveAlpha)
			materialOut.pPixelShaderDepthPass = PixelShader::Resolve(renderer, "cso\\Model_PS(DepthOnly)");
		

		materialOut.Create(renderer, transparent);

	}

	for (int i = 0; i < pScene->mNumMeshes; i++)
	{
		auto mesh = pScene->mMeshes[i];
		shader.mMaterials.at(mesh->mMaterialIndex).indicesToMeshes.push_back(i);
	}


}

void Importer::FillRawVertexBuffer(const aiMesh * mesh, Geometry & mGeometry, std::vector<BoneVertex>& boneVerts, float indexToCBufferMatrix)
{
	using namespace DirectX;
	assert(mGeometry.mRawVertexBuffers.size() > 0);


	for (unsigned int v = 0; v < mesh->mNumVertices; v++)
	{
		for (auto& buffer : mGeometry.mRawVertexBuffers)
		{
			auto numOfAttributes = buffer.GetVertexLayout().GetAttribCount();

			for (const auto& attrib : buffer.GetVertexLayout().GetAttributes())
			{

				DirectX::XMFLOAT4 vert;
				if (indexToCBufferMatrix > -1)
				{
					std::memcpy(&vert, &mesh->mVertices[v], sizeof(DirectX::XMFLOAT3));
					vert.w = indexToCBufferMatrix;
				}

				switch (attrib.GetType())
				{
					case VertexLayout::Position3D:
						buffer.EmplaceBackAttribute<VertexLayout::Position3D>(*reinterpret_cast<XMFLOAT3*>(&mesh->mVertices[v]));
						break;
					case VertexLayout::Position3DPlusIndex:
						buffer.EmplaceBackAttribute<VertexLayout::Position3DPlusIndex>(vert);
						break;
					case VertexLayout::Texture2D:
						buffer.EmplaceBackAttribute<VertexLayout::Texture2D>(*reinterpret_cast<XMFLOAT2*>(&mesh->mTextureCoords[0][v]));
						break;
					case VertexLayout::Normal:
						buffer.EmplaceBackAttribute<VertexLayout::Normal>(*reinterpret_cast<XMFLOAT3*>(&mesh->mNormals[v]));
						break;
					case VertexLayout::Tangent:
						buffer.EmplaceBackAttribute<VertexLayout::Tangent>(*reinterpret_cast<XMFLOAT3*>(&mesh->mTangents[v]));
						break;
					case VertexLayout::Bitangent:
						buffer.EmplaceBackAttribute<VertexLayout::Bitangent>(*reinterpret_cast<XMFLOAT3*>(&mesh->mBitangents[v]));
						break;
					case VertexLayout::Weights:
					{
						assert(boneVerts.size() > 0);
						VertexTypes::Weights weights;
						for (int i = 0; i < 4; i++)
						{
							weights.weight[i] = (unsigned char)(boneVerts[v].weights[i] * 255.0f);
						}
						buffer.EmplaceBackAttribute<VertexLayout::Weights>(weights);
						break;
					}
					case VertexLayout::BoneIndices:
					{
						assert(boneVerts.size() > 0);
						VertexTypes::BoneIndices ind;
						std::memcpy(ind.index, boneVerts.at(v).indicesToBones.data(), sizeof(UINT8) * 4);
						buffer.EmplaceBackAttribute<VertexLayout::BoneIndices>(std::move(ind));
						break;
					}
					default:
						assert("Incorrect element type for import" && false);
				}
			}
		}

		mGeometry.mRawVertices.push_back(*reinterpret_cast<XMFLOAT3*>(&mesh->mVertices[v]));
	}

}


void Importer::FillRawIndexBuffer(const aiMesh * mesh, Geometry & mGeometry)
{
	for (unsigned int i = 0; i < mesh->mNumFaces; i++)
	{
		const auto& face = mesh->mFaces[i];
		assert(face.mNumIndices == 3);
		mGeometry.mRawIndices.push_back(face.mIndices[0]);
		mGeometry.mRawIndices.push_back(face.mIndices[1]);
		mGeometry.mRawIndices.push_back(face.mIndices[2]);
	}
}

