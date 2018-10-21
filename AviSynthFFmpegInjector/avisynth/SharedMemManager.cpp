#include "stdafx.h"
#include "SharedMemManager.h"

SharedMemManager::SharedMemManager(const char* shMemName)
	: sharedMemoryName(shMemName)
{
}

void SharedMemManager::Initialize()
{
	this->hMapFile = OpenFileMappingA(
		FILE_MAP_READ | FILE_MAP_WRITE,
		FALSE,
		this->sharedMemoryName.c_str());
	this->ptrShMem = MapViewOfFile(
		this->hMapFile, // handle to map object
		FILE_MAP_ALL_ACCESS,  // read/write permission
		0,
		0,
		0);
}