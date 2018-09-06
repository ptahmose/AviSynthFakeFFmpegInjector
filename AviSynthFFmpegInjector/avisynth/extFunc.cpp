#include "stdafx.h"

#include "extFunc.h"

void __cdecl avs_bit_blt(AVS_ScriptEnvironment *, AVSI_BYTE* dstp, int dst_pitch, const AVSI_BYTE* srcp, int src_pitch, int row_size, int height)
{
	
}

const char * avs_clip_get_error(AVS_Clip *) // return 0 if no error
{
	return 0;
}

AVS_ScriptEnvironment * avs_create_script_environment(int version)
{
	if (version != 3)
	{
		//set_avs_lasterror()
		return nullptr;
		//return avs_new_value_error("The expected version is \"3\".");
	}
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

const AVS_VideoInfo* avs_get_video_info(AVS_Clip *)
{
	return nullptr;
}

AVS_VideoFrame* avs_get_frame(AVS_Clip *, int n)
{
	return nullptr;
}

int avs_get_version(AVS_Clip *)
{
	return 0;
}

AVS_Value avs_invoke(AVS_ScriptEnvironment *, const char * name, AVS_Value args, const char** arg_names)
{
	if (strcmp(name,"Import")==0)
	{
		if (!avs_is_string(args))
		{
			return avs_new_value_error("Unexpected argument.");
		}

		// now we have to return a "clip"
		AVS_Value v = { 0 };
		v.type = 'c';
		v.d.clip = (void*)1;
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

void avs_release_video_frame(AVS_VideoFrame *)
{
}

AVS_Clip * avs_take_clip(AVS_Value, AVS_ScriptEnvironment *)
{
	return nullptr;
}

int avs_bits_per_pixel(const AVS_VideoInfo * p)
{
	return 0;
}

int avs_get_height_p(const AVS_VideoFrame * p, int plane)
{
	return 0;
}

int avs_get_pitch_p(const AVS_VideoFrame * p, int plane)
{
	return 0;
}

const BYTE * avs_get_read_ptr_p(const AVS_VideoFrame * p, int plane)
{
	return nullptr;
}

int avs_get_row_size_p(const AVS_VideoFrame * p, int plane)
{
	return 0;
}

int avs_is_planar_rgb(const AVS_VideoInfo * p)
{
	return 0;
}

int avs_is_planar_rgba(const AVS_VideoInfo * p)
{
	return 0;
}

