#pragma once
#include "sharedmemstructures.h"
#include "hptimer.h"
#include <stdexcept>
#include <functional>
#include <stdexcept>
#include <cstdint>

struct FrameLockInfo
{
	int frameNo;
};

class SharedMemHelperReader
{
private:
	SharedMemHdr* hdr;

	SharedMemHelperReader() {};
public:

	/// <summary>
	/// Constructor for the SharedMemHelperReader. It is assumed that the pointer to the SharedMemHdr-struct
	/// remains valid for the lifetime of this object.
	/// </summary>
	/// <exception cref="std::invalid_argument">Thrown when an invalid argument error condition
	/// occurs.</exception>
	/// <param name="hdr">[in,out] If non-null, the header.</param>
	SharedMemHelperReader(SharedMemHdr* hdr) : hdr(hdr)
	{
		if (hdr->magic != SharedMemMagic)
		{
			throw std::invalid_argument("Invalid magic.");
		}
	}

	bool Probe();

	const SharedMemVideoInfo& GetVideoInfo()
	{
		return this->hdr->videoInfo;
	}

	bool TryLockNextFrameWithTightLoopAndSlowLoop(FrameLockInfo& lockInfo, uint32_t maxWaitTightLoop, uint32_t maxWaitSlowLoop)
	{
		bool b = this->TryLockNextFrameWithTimeout(lockInfo, maxWaitTightLoop);
		if (b == true)
		{
			return b;
		}

		// TODO: would "WaitOnAddress" work here? ( https://docs.microsoft.com/en-us/windows/desktop/api/synchapi/nf-synchapi-waitonaddress )
		HpTimer timer;
		for (;;)
		{
			Sleep(1);
			b = this->TryLockNextFrame(lockInfo);
			if (b == true)
			{
				return b;
			}

			if (timer.GetElapsedTime() >= maxWaitSlowLoop)
			{
				return false;
			}
		}
	}

	bool TryLockNextFrameWithTimeout(FrameLockInfo& lockInfo, uint32_t maxWait)
	{
		bool b = this->TryLockNextFrame(lockInfo);
		if (b == true)
		{
			return b;
		}

		if (maxWait > 0)
		{
			HpTimer timer;
			for (;;)
			{
				b = this->TryLockNextFrame(lockInfo);
				if (b == true)
				{
					return b;
				}

				YieldProcessor();
				if (timer.GetElapsedTime() >= maxWait)
				{
					return false;
				}
			}
		}

		return false;
	}

	bool IsNextFrameAvailable()
	{
		int curReadPtr = this->hdr->videoBufferState.readPtr;
		if (InterlockedCompareExchange(
			(LONG*)&this->hdr->videoBufferState.bufferStates[curReadPtr],
			BufferState_InUse,
			BufferState_Filled) == BufferState_Filled)
		{
			return true;
		}

		return false;
	}

	bool TryLockNextFrame(FrameLockInfo& lockInfo)
	{
		int curReadPtr = this->hdr->videoBufferState.readPtr;
		if (InterlockedCompareExchange(
			(LONG*)&this->hdr->videoBufferState.bufferStates[curReadPtr],
			BufferState_InUse,
			BufferState_Filled) == BufferState_Filled)
		{
			lockInfo.frameNo = curReadPtr;
			return true;
		}

		return false;
	}

	const void* GetLockedFramePointer(const FrameLockInfo& lockInfo)
	{
		const void* ptr = ((const char*)this->hdr) + this->hdr->videoBufferInfo.offset[lockInfo.frameNo];
		return ptr;
	}

	void FreeLockedFrame(const FrameLockInfo& lockInfo)
	{
		if (lockInfo.frameNo != this->hdr->videoBufferState.readPtr)
		{
			throw std::runtime_error("inconsistent state");
		}

		if (this->hdr->videoBufferState.bufferStates[lockInfo.frameNo] != BufferState_InUse)
		{
			throw std::runtime_error("inconsistent buffer-state");
		}

		this->hdr->videoBufferState.bufferStates[lockInfo.frameNo] = BufferState_Free;
		this->hdr->videoBufferState.readPtr = this->GetIncrementedReadPtr();
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
		if (rp >= this->hdr->videoBufferInfo.videoBufferCount)
		{
			rp = 0;
		}

		return rp;
	}

private:
	static bool IsValidPixelType(int pixelType);
	static bool IsStrideValid(int pixelType, std::uint32_t width, std::uint32_t stride);
};
