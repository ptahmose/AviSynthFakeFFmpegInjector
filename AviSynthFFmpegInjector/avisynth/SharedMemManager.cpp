#include "stdafx.h"
#include "SharedMemManager.h"

using namespace std;

SharedMemManager::SharedMemManager(const char* shMemName)
	: sharedMemoryName(shMemName),
	hMapFile(INVALID_HANDLE_VALUE),
	ptrShMem(nullptr)
{
}

bool SharedMemManager::Initialize()
{
	if (this->hMapFile != INVALID_HANDLE_VALUE)
	{
		throw logic_error("invalid state");
	}

	HANDLE h = OpenFileMappingA(
		FILE_MAP_READ | FILE_MAP_WRITE,
		FALSE,
		this->sharedMemoryName.c_str());

	if (h == INVALID_HANDLE_VALUE || h == NULL)	// https://docs.microsoft.com/en-us/windows/desktop/api/winbase/nf-winbase-openfilemappinga claims that we get a NULL in case of error
	{
		return false;
	}

	LPVOID mappedAddress = MapViewOfFile(
		h,						// handle to map object
		FILE_MAP_ALL_ACCESS,	// read/write permission
		0,
		0,
		0);
	if (mappedAddress == NULL)
	{
		CloseHandle(h);
		return false;
	}

	this->hMapFile = h;
	this->ptrShMem = mappedAddress;
	return true;
}