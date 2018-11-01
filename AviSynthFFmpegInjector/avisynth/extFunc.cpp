#include "stdafx.h"

#include "extFunc.h"
#include "Utils.h"
#include <sstream>
#include "avisynth_c.h"
#include "SharedMemHelper.h"
#include "SharedMemManager.h"
#include "avsclipobj.h"

using namespace std;

struct AVS_ScriptEnvironment
{

};

struct AVS_Clip
{
	SharedMemHelperReader* smHlp;
	SharedMemManager* sharedMemManager;
	AVS_VideoInfo videoInfo;
};

struct AVS_VideoFrameEx :AVS_VideoFrame
{
	int pixeltype;
	int width, height;
	FrameLockInfo lockInfo;

	// TODO: use a shared_ptr
	AVS_Clip* clip;
};

/*
static uint8_t val=0;
static int c = 0;
void __cdecl avs_bit_blt(AVS_ScriptEnvironment *, AVSI_BYTE* dstp, int dst_pitch, const AVSI_BYTE* srcp, int src_pitch, int row_size, int height)
{

	val = 0;
	for (int y=0;y<height;++y)
	{
		uint8_t* ptrDst = dstp + y * dst_pitch;
		for (int x=0;x<row_size;x+=3)
		{
			if (c == 0)
			{
				ptrDst[x] = val++;
				ptrDst[x + 1] = 0;
				ptrDst[x + 2] = 0;
			}
			else if (c==1)
			{
				ptrDst[x] = 0;
				ptrDst[x + 1] = val++;
				ptrDst[x + 2] = 0;
			}
			else
			{
				ptrDst[x] = 0;
				ptrDst[x + 1] = 0;
				ptrDst[x + 2] = val++;
			}
		}
	}

	c = c + 1;
	c = c % 3;
}
*/
void __cdecl avs_bit_blt(AVS_ScriptEnvironment *, AVSI_BYTE* dstp, int dst_pitch, const AVSI_BYTE* srcp, int src_pitch, int row_size, int height)
{
	// avisynth.c line 681-685 has the habit of vertically flipping the image for RGB24/RGB32.
	// We compensate for this here.

	if (src_pitch < 0)
	{
		srcp += (height - 1)*src_pitch;

		src_pitch = -src_pitch;
	}

	for (int i = 0; i < height; ++i)
	{
		memcpy(dstp, srcp, row_size);
		dstp += dst_pitch;
		srcp += src_pitch;
	}
}

const char * avs_clip_get_error(AVS_Clip *) // return 0 if no error
{
	Log(LogLevel_Trace,
		[=](void)->string
	{
		stringstream ss;
		ss << "Entered \"avs_clip_get_error( [clip] )" << endl;
		return ss.str();
	});

	// TODO: that's a way to report "end-of-file" or so - if we return an error, the encoding is stopped it seems

	const char* szErrorText = "TESTTEXT";

	szErrorText = nullptr;

	return szErrorText;
}

AVS_ScriptEnvironment * avs_create_script_environment(int version)
{
	Log(LogLevel_Trace,
		[=](void)->string
	{
		stringstream ss;
		ss << "Entered \"avs_create_script_environment(" << version << ")" << endl;
		return ss.str();
	});

	MessageBoxA(HWND_DESKTOP, "Waiting for Debugger to attach", "Waiting...", MB_OK);

	if (version != 3)
	{
		//set_avs_lasterror()
		return nullptr;
		//return avs_new_value_error("The expected version is \"3\".");
	}

	return new AVS_ScriptEnvironment();
}

void avs_delete_script_environment(AVS_ScriptEnvironment *)
{
}

int avs_get_audio(AVS_Clip *, void * buf, INT64 start, INT64 count)
{
	return 0;
}
const char * avs_get_error(AVS_ScriptEnvironment *) // return 0 if no error
{
	return 0;
}

const AVS_VideoInfo* avs_get_video_info(AVS_Clip * avsClip)
{
	Log(LogLevel_Trace,
		[=](void)->string
	{
		stringstream ss;
		ss << "Entered \"avs_get_video_info(clip)\"" << endl;
		return ss.str();
	});

	memset(&avsClip->videoInfo, 0, sizeof(avsClip->videoInfo));
	avsClip->videoInfo.width = 1920;
	avsClip->videoInfo.height = 1080;
	avsClip->videoInfo.fps_numerator = 25;
	avsClip->videoInfo.fps_denominator = 1;
	avsClip->videoInfo.pixel_type = AVS_CS_BGR24;
	avsClip->videoInfo.audio_samples_per_second = 0;
	avsClip->videoInfo.num_frames = 1000*10;
	return &avsClip->videoInfo;
}

AVS_VideoFrame* avs_get_frame(AVS_Clip* clip, int n)
{
	Log(LogLevel_Trace,
		[=](void)->string
	{
		stringstream ss;
		ss << "Entered \"avs_get_frame( [clip], " << n << ")\"" << endl;
		return ss.str();
	});

	FrameLockInfo lockInfo;
	for (;;)
	{
		bool b = clip->smHlp->TryLockNextFrame(lockInfo);
		if (b == true)
		{
			break;
		}
	}

	AVS_VideoFrameEx* videoFrame = new AVS_VideoFrameEx();
	videoFrame->pitch = ((1920 * 3 + 3) / 4) * 4;
	videoFrame->width = 1920;
	videoFrame->height = 1080;
	videoFrame->lockInfo = lockInfo;
	videoFrame->clip = clip;
	return videoFrame;
}

int avs_get_version(AVS_Clip *)
{
	return 6;
}

AVS_Value avs_invoke(AVS_ScriptEnvironment *, const char * name, AVS_Value args, const char** arg_names)
{
	Log(LogLevel_Trace,
		[=](void)->string
	{
		stringstream ss;
		ss << "Entered \"avs_invoke( [env], " << name << ", args, arg_names )";
		return ss.str();
	});

	if (strcmp(name, "Import") == 0)
	{
		if (!avs_is_string(args))
		{
			return avs_new_value_error("Unexpected argument.");
		}

		AvsClipObj* clip = new AvsClipObj();
		clip->InitFromFile(avs_as_string(args));

		// now we have to return a "clip"
		AVS_Value v = { 0 };
		v.type = 'c';
		v.d.clip = new AVS_Clip();
		return v;
	}

	return avs_void;
}

void avs_release_clip(AVS_Clip *)
{
}

void avs_release_value(AVS_Value)
{
}

void avs_release_video_frame(AVS_VideoFrame* p)
{
	Log(LogLevel_Trace,
		[=](void)->string
	{
		stringstream ss;
		ss << "Entered \"avs_release_video_frame( [videoframe] )" << endl;
		return ss.str();
	});

	AVS_VideoFrameEx* videoFrame = (AVS_VideoFrameEx*)p;
	videoFrame->clip->smHlp->FreeLockedFrame(videoFrame->lockInfo);
	delete p;
}

AVS_Clip * avs_take_clip(AVS_Value, AVS_ScriptEnvironment *)
{
	MessageBoxA(HWND_DESKTOP, "Waiting for Debugger to attach", "Waiting...", MB_OK);

	auto shrdMemManager = new SharedMemManager("SHAREDMEMTEST");
	shrdMemManager->Initialize();
	auto shrdMemHlp = new SharedMemHelperReader(shrdMemManager->GetSharedMemHdrPointer());

	auto avsClip = new AVS_Clip();
	avsClip->sharedMemManager = shrdMemManager;
	avsClip->smHlp = shrdMemHlp;
	return avsClip;

	//return new AVS_Clip();
}

int avs_bits_per_pixel(const AVS_VideoInfo * p)
{
	Log(LogLevel_Trace,
		[=](void)->string
	{
		stringstream ss;
		ss << "Entered \"avs_bits_per_pixel( [videoinfo] )" << endl;
		return ss.str();
	});

	// TODO: check if video is valid (width&height != 0 I reckon)

	switch (p->pixel_type)
	{
	case AVS_CS_BGR24:
		return 3 * 8;
	default:
		// TODO: proper error-handling
		return -1;
	};
}

int avs_get_height_p(const AVS_VideoFrame * p, int plane)
{
	Log(LogLevel_Trace,
		[=](void)->string
	{
		stringstream ss;
		ss << "Entered \"avs_get_height_p( [videoframe], " << plane << " )" << endl;
		return ss.str();
	});

	AVS_VideoFrameEx* videoFrame = (AVS_VideoFrameEx*)p;
	auto videoInfo = videoFrame->clip->smHlp->GetVideoInfo();
	return videoInfo.height;
	//return videoFrame->height;

}

int avs_get_pitch_p(const AVS_VideoFrame * p, int plane)
{
	Log(LogLevel_Trace,
		[=](void)->string
	{
		stringstream ss;
		ss << "Entered \"avs_get_pitch_p( [videoframe], " << plane << " )" << endl;
		return ss.str();
	});

	AVS_VideoFrameEx* videoFrame = (AVS_VideoFrameEx*)p;
	auto videoInfo = videoFrame->clip->smHlp->GetVideoInfo();
	return videoInfo.stride;
	//return videoFrame->pitch;
}

const BYTE * avs_get_read_ptr_p(const AVS_VideoFrame * p, int plane)
{
	AVS_VideoFrameEx* videoFrame = (AVS_VideoFrameEx*)p;
	auto ptr = (const BYTE *)(videoFrame->clip->smHlp->GetLockedFramePointer(videoFrame->lockInfo));
	return ptr;
}

int avs_get_row_size_p(const AVS_VideoFrame * p, int plane)
{
	Log(LogLevel_Trace,
		[=](void)->string
	{
		stringstream ss;
		ss << "Entered \"avs_get_row_size_p( [videoframe], " << plane << " )" << endl;
		return ss.str();
	});

	AVS_VideoFrameEx* videoFrame = (AVS_VideoFrameEx*)p;

	auto videoInfo = videoFrame->clip->smHlp->GetVideoInfo();

	// TODO
	return videoInfo.width * 3;

	//return videoFrame->width*3;
}

int avs_is_planar_rgb(const AVS_VideoInfo * p)
{
	// Note: the presence of this method is used by ffmpeg in order to decided whether >= AviSynth 2.6 or AviSynth+
	Log(LogLevel_Trace,
		[=](void)->string
	{
		stringstream ss;
		ss << "Entered \"avs_is_planar_rgb( [videoinfo] )" << endl;
		return ss.str();
	});

	return 0;
}

int avs_is_planar_rgba(const AVS_VideoInfo * p)
{
	Log(LogLevel_Trace,
		[=](void)->string
	{
		stringstream ss;
		ss << "Entered \"avs_is_planar_rgba( [videoinfo] )" << endl;
		return ss.str();
	});

	return 0;
}

