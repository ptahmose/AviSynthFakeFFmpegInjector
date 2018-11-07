#pragma once

#include "SharedMemHelper.h"
#include "avisynth_c.h"

class AvsClipObj;

class AvsVideoFrame
{
private:
	FrameLockInfo frameLockInfo;
public:
	AvsVideoFrame();
};


struct AVSVideoFrameEx : AVS_VideoFrame
{
	AvsClipObj*	clipObj;
	FrameLockInfo frameLockInfo;
};