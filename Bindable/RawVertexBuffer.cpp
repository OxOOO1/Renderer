#include "RawVertexBuffer.h"
#include "src/System/Utilities/Exporter.h"
#include "src/System/Utilities/Importer.h"


void RawVertexBuffer::WriteToFile(FileIO::WriteToFile& file)
{
	//mVertexLayout.WriteToFile(file);
	Exporter::WriteVecToFile(file, buffer);
}

void RawVertexBuffer::ReadFromFile(FileIO::ReadFromFile& file)
{
	//mVertexLayout.ReadFromFile(file);
	Importer::ReadVecFromFile(file, buffer);
}