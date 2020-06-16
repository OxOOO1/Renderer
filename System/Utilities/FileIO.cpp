#include "FileIO.h"

FileIO::ReadFromFile::ReadFromFile(const char * name)
{

	hFile = CreateFile(
		name,
		GENERIC_READ,
		FILE_SHARE_READ,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL
	);

	if (hFile == INVALID_HANDLE_VALUE)
	{
		bClosed = true;
	}

}

void FileIO::ReadFromFile::Read(void * buffer, unsigned int numOfBytesToRead)
{

	bool er = ReadFile(hFile, buffer, numOfBytesToRead, &dwBytesRead, NULL);

	/*if (!er || dwBytesRead != numOfBytesToRead)
	{
	CloseHandle(hFile);
	throw WND_EXCEPT_LAST(void);
	}*/

}

FileIO::WriteToFile::WriteToFile(const char * name)
{

	hFile = CreateFile(
		name,
		GENERIC_WRITE,
		0,
		NULL,
		TRUNCATE_EXISTING,
		FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN,
		NULL);

	if (hFile == INVALID_HANDLE_VALUE)
	{
		if (GetLastError() == ERROR_FILE_NOT_FOUND)
		{
			hFile = CreateFile(
				name,
				GENERIC_WRITE,
				0,
				NULL,
				CREATE_NEW,
				FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN,
				NULL);

			if (hFile == INVALID_HANDLE_VALUE)
			{
				throw WND_EXCEPT_LAST(void);
			}

		}
	}

}

void FileIO::WriteToFile::Write(void * buffer, unsigned int numOfBytesToWrite)
{
	bool bErrorFlag = WriteFile(
		hFile,
		buffer,
		numOfBytesToWrite,
		&dwBytesWritten,
		NULL
	);

	if (FALSE == bErrorFlag)
	{
		throw WND_EXCEPT_LAST(void);
	}

}
