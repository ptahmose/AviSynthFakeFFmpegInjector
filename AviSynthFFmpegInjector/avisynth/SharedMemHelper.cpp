#include "stdafx.h"
#include "SharedMemHelper.h"
#include "avisynth_c.h"

using namespace std;

bool SharedMemHelperReader::Probe()
{
	// validate videoInfo
	bool valid =
		SharedMemHelperReader::IsValidPixelType(this->hdr->videoInfo.pixelType) &&
		this->hdr->videoInfo.width > 0 &&
		this->hdr->videoInfo.height > 0 &&
		SharedMemHelperReader::IsStrideValid(this->hdr->videoInfo.pixelType, this->hdr->videoInfo.width, this->hdr->videoInfo.stride);
		valid = valid &&
		this->hdr->videoInfo.fps_numerator > 0 &&
		this->hdr->videoInfo.fps_denominator;

	// check videoBufferInfo
	valid = valid &&
		this->hdr->videoBufferInfo.videoBufferCount <= MAX_VIDEO_BUFFERS;

	// TODO: 
	// - check the offsets
	// - try to read the end (and watch out for an access violation)

	return true;
}


/*static*/bool SharedMemHelperReader::IsValidPixelType(int pixelType)
{
	switch (pixelType)
	{
	case AVS_CS_BGR24:
	case AVS_CS_BGR32:
		return true;
	}

	return false;
}

/*static*/bool SharedMemHelperReader::IsStrideValid(int pixelType, uint32_t width, uint32_t stride)
{
	uint64_t minStride=0;
	switch (pixelType)
	{
	case AVS_CS_BGR24:
		minStride = width * 3ULL;
		break;
	case AVS_CS_BGR32:
		minStride = width * 4ULL;
		break;
	}

	if (minStride == 0 || minStride > stride)
	{
		return false;
	}

	return true;
}