#include "Exporter.h"
#include "Importer.h"

void Exporter::Export(Model & model, const std::string & name)
{
	FileIO::WriteToFile ofile(std::string{MainFilePath + name + ".asset" }.c_str());
	std::ofstream jbin(MainFilePath + name + ".meta");
	nlohmann::json jj;

	jj["ModelName"] = name;
	jj["ModelTypeID"] = model.GetType();

	ExportGeometry(model.mGeometry, ofile);
	ExportMaterials(model.mShading, jj);

	jbin << jj;

}

void Exporter::ExportGeometry(Geometry & mGeometry, FileIO::WriteToFile & file)
{

	WriteVecToFile(file, mGeometry.mMeshes);
	WriteVecToFile(file, mGeometry.mRawVertices);
	WriteVecToFile(file, mGeometry.mRawIndices);

	for (auto& buffer : mGeometry.mRawVertexBuffers)
	{
		buffer.WriteToFile(file);
	}

}

void Exporter::ExportMaterials(Shading & shading, nlohmann::json & meta)
{
	meta["NumOfMaterials"] = shading.mMaterials.size();

	int count = 0;
	for (auto& mat : shading.mMaterials)
	{
		meta[std::to_string(count)]["Name"] = mat.mName;
		if (mat.mDesc.hasDiffuse)
			meta[mat.mName]["Diffuse"] = mat.mFileNames.diffuse;
		if (mat.mDesc.hasNormal)
			meta[mat.mName]["Normal"] = mat.mFileNames.normal;
		if (mat.mDesc.hasSpecular)
			meta[mat.mName]["Specular"] = mat.mFileNames.specular;
		if (mat.mDesc.isEmitter)
			meta[mat.mName]["Emission"] = mat.mFileNames.emissive;

		meta[mat.mName]["HasAlpha"] = mat.mDesc.hasActiveAlpha;
		meta[mat.mName]["Transparent"] = mat.mDesc.transparent;

		meta[mat.mName]["IndicesToMeshes"] = mat.indicesToMeshes;

		count++;
	}
}
