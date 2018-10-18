#pragma once
#include "sharedmemstructures.h"
#include <stdexcept>
#include <functional>

class SharedMemHelperReader
{
private:
	SharedMemHdr* hdr;
public:
	SharedMemHelperReader(SharedMemHdr* hdr) : hdr(hdr)
	{
		if (hdr->magic != SharedMemMagic)
		{
			throw std::invalid_argument("Invalid magic.");
		}
	}

	const SharedMemVideoInfo& GetVideoInfo()
	{
		return this->hdr->videoInfo;
	}


	bool GetNextFrame(const std::function<void(const void* ptr)>& operationOnBuffer)
	{
		int curReadPtr = this->hdr->videoBufferState.readPtr;
		if (InterlockedCompareExchange(
			(LONG*)&this->hdr->videoBufferState.bufferStates[curReadPtr],
			BufferState_InUse,
			BufferState_Filled) == BufferState_Filled)
		{
			operationOnBuffer(
				((const char*)this->hdr) + this->hdr->videoBufferInfo.offset[curReadPtr]);

			this->hdr->videoBufferState.bufferStates[curReadPtr] = BufferState_Free;
			this->hdr->videoBufferState.readPtr = this->GetIncrementedReadPtr();
			return true;
		}

		return false;
	}

	int GetIncrementedReadPtr()
	{
		int rp = this->hdr->videoBufferState.readPtr;
		rp++;
		if (rp>=this->hdr->videoBufferInfo.videoBufferCount)
		{
			rp = 0;
		}

		return rp;
	}
};
