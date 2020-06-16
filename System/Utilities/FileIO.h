#pragma once
#include "System/Window.h"

namespace FileIO {

	class ReadFromFile
	{
	public:
		ReadFromFile(const char* name);

		void Read(void* to, unsigned int numOfBytesToRead);

		template<typename T>
		void Read(T* to)
		{
			Read(to, sizeof(T));
		}

		void Close()
		{
			CloseHandle(hFile);
			bClosed = true;
		}

		bool IsOpen() const
		{
			return !bClosed;
		}

		~ReadFromFile()
		{
			if (!bClosed)
				CloseHandle(hFile);
		}

	private:
		HANDLE hFile;
		DWORD dwBytesRead = 0;
		bool bClosed = false;
	};

	class WriteToFile
	{
	public:
		WriteToFile(const char* name);
		WriteToFile(const WriteToFile&) = delete;

		void Write(void* from, unsigned int numOfBytesToWrite);

		template<typename T>
		void Write(T* from)
		{
			Write(from, sizeof(T));
		}

		void Close()
		{
			CloseHandle(hFile);
			closed = true;
		}

		~WriteToFile()
		{
			if (!closed)
			{
				CloseHandle(hFile);
			}
		}

	private:
		HANDLE hFile;
		DWORD dwBytesWritten = 0;
		bool closed = false;
	};


}







