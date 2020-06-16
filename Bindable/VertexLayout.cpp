#include "VertexLayout.h"
#include "src/System/Utilities/Exporter.h"
#include "src/System/Utilities/Importer.h"

void VertexLayout::WriteToFile(FileIO::WriteToFile & file)
{
	Exporter::WriteVecToFile(file, attributesDescs);
}

void VertexLayout::ReadFromFile(FileIO::ReadFromFile & file)
{
	Importer::ReadVecFromFile(file, attributesDescs);
}
