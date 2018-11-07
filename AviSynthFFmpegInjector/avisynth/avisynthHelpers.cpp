#include "stdafx.h"
#include "avisynthHelpers.h"
#include "avisynth_c.h"

int CalcLengthOfRow(int width, int pixelType)
{
	return GetBytesPerPixel(pixelType)*width;
}

int GetBytesPerPixel(int pixelType)
{
	switch (pixelType)
	{
	case AVS_CS_BGR24:
		return 3;
	default:
		// TODO: proper error-handling
		return -1;
	};
}