#pragma once

#include <pshpack4.h>
#include <cstdint>

const int MAX_VIDEO_BUFFERS = 8;

struct SharedMemVideoInfo
{
	int				pixelType;
	std::uint32_t	width;
	std::uint32_t	height;
	std::uint32_t	stride;

	std::uint32_t	fps_numerator;
	std::uint32_t	fps_denominator;

	/// <summary>
	/// Specifies the number of frames that will be added. 0 means "undetermined number of frames".
	/// </summary>
	std::uint32_t	numberOfFrames;	
};

const int BufferState_InUse = 1;
const int BufferState_Filled = 2;
const int BufferState_Free = 3;

struct VideoBufferInfo
{
	int videoBufferCount;
	std::uint32_t offset[MAX_VIDEO_BUFFERS];
};

struct BufferState
{
	int readPtr;
	int writePtr;

	int bufferStates[MAX_VIDEO_BUFFERS];
};

struct SharedMemHdr
{
	GUID	magic;

	SharedMemVideoInfo	videoInfo;

	VideoBufferInfo videoBufferInfo;
	BufferState videoBufferState;
};

// {1EB32E5E-56B9-466E-9C98-27E3933DBD23}
static const GUID SharedMemMagic =
{ 0x1eb32e5e, 0x56b9, 0x466e, { 0x9c, 0x98, 0x27, 0xe3, 0x93, 0x3d, 0xbd, 0x23 } };


#include <poppack.h>