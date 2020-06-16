#include "Skybox.h"

#include "System/Utilities/FileIO.h"

#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"

#include "Bindable/VertexBuffer.h"
#include "src/System/Utilities/Importer.h"

Skybox::Skybox(Rendering & renderer, std::string_view fileName)
	:
	mShading{renderer,"cso\\Skybox_PS" },
	mGeometry{renderer, "cso\\Skybox_VS", "",
{{VertexLayout::Position3D, VertexLayout::Texture2D}}
}
{
	using namespace std::string_literals;
	Material mat;
	mat.mDesc.hasDiffuse = true;
	std::string name = fileName.data();
	mat.AddTexture(Texture::Resolve(renderer, MainFilePath + "skyboxes\\"s + name, Texture::DiffuseSlot));
	mat.mFileNames.diffuse = name;
	mat.Create(renderer);
	mShading.mMaterials.push_back(std::move(mat));
}
