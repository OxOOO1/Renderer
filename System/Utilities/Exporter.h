#pragma once

#include "src/Components/Drawable/Components/Geometry.h"
#include "src/Components/Drawable/Components/Shading.h"
#include "src/Components/Drawable/Model.h"

#include "FileIO.h"
#include"nlohmann/json.hpp"
#include <fstream>

//TODO Boost::serialize 

class Exporter
{
public:

	static void Export(Model& model, const std::string& name);

	template<typename T>
	static void WriteVecToFile(FileIO::WriteToFile& file, std::vector<T>& vec)
	{
		size_t size = vec.size();
		file.Write(&size);
		file.Write(vec.data(), size * sizeof(T));
	}

private:
	static void ExportGeometry(Geometry& mGeometry, FileIO::WriteToFile& file);
	static void ExportMaterials(Shading& shading, nlohmann::json& meta);




};