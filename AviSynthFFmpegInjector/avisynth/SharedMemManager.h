#pragma once

#include <string>
#include "sharedmemstructures.h"

class SharedMemManager
{
private:
	HANDLE hMapFile;
	std::string sharedMemoryName;
	void* ptrShMem;
public:
	SharedMemManager(const char* shMemName);

	bool Initialize();

	SharedMemHdr* GetSharedMemHdrPointer() { return (SharedMemHdr*)this->ptrShMem; }
};
