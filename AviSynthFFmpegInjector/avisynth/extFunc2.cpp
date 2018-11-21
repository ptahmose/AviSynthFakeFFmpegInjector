#include "stdafx.h"
#include "extFunc.h"
#include <sstream>
#include <Windows.h>
#include "Utils.h"
#include "avsScriptEnvironment.h"
#include "avsclipobj.h"
#include "avisynth_c.h"
#include "avisynthHelpers.h"
//#include "Utils.h"
//#include <sstream>
//#include "avisynth_c.h"
//#include "SharedMemHelper.h"
//#include "SharedMemManager.h"
//#include "avsclipobj.h"
//#include "avsScriptEnvironment.h"

using namespace std;

AVS_ScriptEnvironment* avs_create_script_environment(int version)
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
		return nullptr;
	}

	return (AVS_ScriptEnvironment*)new AvsScriptEnvironment();
}

void avs_delete_script_environment(AVS_ScriptEnvironment* p)
{
	AvsScriptEnvironment* scriptEnv = (AvsScriptEnvironment*)p;
	delete scriptEnv;
}

AVS_Value avs_invoke(AVS_ScriptEnvironment* p, const char * name, AVS_Value args, const char** arg_names)
{
	Log(LogLevel_Trace,
		[=](void)->string
	{
		stringstream ss;
		ss << "Entered \"avs_invoke( [env], " << name << ", args, arg_names )\"" << endl;
		return ss.str();
	});

	AvsScriptEnvironment* scriptEnv = (AvsScriptEnvironment*)p;

	bool isSane = scriptEnv->CheckSaneState();
	if (!isSane)
	{
		return avs_new_value_error("Corrupted state.");
	}

	if (strcmp(name, "Import") == 0)
	{
		if (!avs_is_string(args))
		{
			return avs_new_value_error("Unexpected argument.");
		}

		AvsClipObj* clip = new AvsClipObj();
		clip->InitFromFile(avs_as_string(args));

		clip->InitializeSharedMem();

		// now we have to return a "clip"
		AVS_Value v = { 0 };
		v.type = 'c';
		v.d.clip = clip;
		return v;
	}

	return avs_void;
}

const char* avs_clip_get_error(AVS_Clip* p) // return 0 if no error
{
	Log(LogLevel_Trace,
		[=](void)->string
	{
		stringstream ss;
		ss << "Entered \"avs_clip_get_error( [clip] )" << endl;
		return ss.str();
	});

	AvsClipObj* clipObj = (AvsClipObj*)p;

	return clipObj->GetLastError();
}

int avs_get_audio(AVS_Clip *, void * buf, INT64 start, INT64 count)
{
	return 0;
}

const char * avs_get_error(AVS_ScriptEnvironment*) // return 0 if no error
{
	return 0;
}

const AVS_VideoInfo* avs_get_video_info(AVS_Clip* p)
{
	Log(LogLevel_Trace,
		[=](void)->string
	{
		stringstream ss;
		ss << "Entered \"avs_get_video_info(clip)\"" << endl;
		return ss.str();
	});

	AvsClipObj* clipObj = (AvsClipObj*)p;
	return clipObj->GetVideoInfo(0);
	//memset(&avsClip->videoInfo, 0, sizeof(avsClip->videoInfo));
	//avsClip->videoInfo.width = 1920;
	//avsClip->videoInfo.height = 1080;
	//avsClip->videoInfo.fps_numerator = 25;
	//avsClip->videoInfo.fps_denominator = 1;
	//avsClip->videoInfo.pixel_type = AVS_CS_BGR24;
	//avsClip->videoInfo.audio_samples_per_second = 0;
	//avsClip->videoInfo.num_frames = 1000 * 10;
	//return &avsClip->videoInfo;
}

AVS_VideoFrame* avs_get_frame(AVS_Clip* p, int n)
{
	Log(LogLevel_Trace,
		[=](void)->string
	{
		stringstream ss;
		ss << "Entered \"avs_get_frame( [clip], " << n << ")\"" << endl;
		return ss.str();
	});

	AvsClipObj* clipObj = (AvsClipObj*)p;

	auto vf = clipObj->GetVideoFrame(n);

	return vf;

	/*
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
	return nullptr;*/
}

int avs_get_version(AVS_Clip *)
{
	return 6;
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

	AVSVideoFrameEx* vf = (AVSVideoFrameEx*)p;
	vf->clipObj->ReleaseCurVideoFrame();
	//AVS_VideoFrameEx* videoFrame = (AVS_VideoFrameEx*)p;
	//videoFrame->clip->smHlp->FreeLockedFrame(videoFrame->lockInfo);
	//delete p;
}


void avs_bit_blt(AVS_ScriptEnvironment *, AVSI_BYTE* dstp, int dst_pitch, const AVSI_BYTE* srcp, int src_pitch, int row_size, int height)
{
	for (int i = 0; i < height; ++i)
	{
		memcpy(dstp, srcp, row_size);
		dstp += dst_pitch;
		srcp += src_pitch;
	}
}

AVS_Clip* avs_take_clip(AVS_Value value, AVS_ScriptEnvironment *)
{
	if (!avs_is_clip(value) || value.d.clip == nullptr)
	{
		return nullptr;
	}

	return (AVS_Clip*)value.d.clip;
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

	AVSVideoFrameEx* vf = (AVSVideoFrameEx*)p;
	return vf->clipObj->GetVideoInfo(0)->height;
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

	AVSVideoFrameEx* vf = (AVSVideoFrameEx*)p;

	return vf->clipObj->GetCurVideoFrameStride(vf);
}

const BYTE* avs_get_read_ptr_p(const AVS_VideoFrame * p, int plane)
{
	AVSVideoFrameEx* vf = (AVSVideoFrameEx*)p;
	return (const BYTE*)vf->clipObj->GetCurVideoFramePointer(vf);
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

	AVSVideoFrameEx* vf = (AVSVideoFrameEx*)p;
	auto vi = vf->clipObj->GetVideoInfo(0);
	return CalcLengthOfRow(vi->width, vi->pixel_type);
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

EXTERN_C int avs_is_planar_rgba(const AVS_VideoInfo * p)
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

/*
* avs_bit_blt
* avs_clip_get_error
* avs_create_script_environment
* avs_delete_script_environment
* avs_get_audio
* avs_get_error
* avs_get_frame
* avs_get_version
* avs_get_video_info
* avs_invoke
* avs_release_clip
* avs_release_value
* avs_release_video_frame
* avs_take_clip
* avs_bits_per_pixel
* avs_get_height_p
* avs_get_pitch_p
* avs_get_read_ptr_p
* avs_get_row_size_p
avs_is_planar_rgb
avs_is_planar_rgba
*/