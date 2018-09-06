#pragma once

#include "importexport.h"

struct AVS_ScriptEnvironment;
struct AVS_Clip;

typedef unsigned char   AVSI_BYTE;

// AVS_Value is layed out identicly to AVSValue
typedef struct AVS_Value AVS_Value;
struct AVS_Value {
	short type;  // 'a'rray, 'c'lip, 'b'ool, 'i'nt, 'f'loat, 's'tring, 'v'oid, or 'l'ong
				 // for some function e'rror
	short array_size;
	union {
		void * clip; // do not use directly, use avs_take_clip
		char boolean;
		int integer;
		float floating_pt;
		const char * string;
		const AVS_Value * array;
	} d;
};

// AVS_Value should be initilized with avs_void.
// Should also set to avs_void after the value is released
// with avs_copy_value.  Consider it the equalvent of setting
// a pointer to NULL
static const AVS_Value avs_void = { 'v' };

// --------------------------------------------------------------------------

// AVS_VideoFrameBuffer is layed out identicly to VideoFrameBuffer
// DO NOT USE THIS STRUCTURE DIRECTLY
typedef struct AVS_VideoFrameBuffer {
	BYTE * data;
	int data_size;
	// sequence_number is incremented every time the buffer is changed, so
	// that stale views can tell they're no longer valid.
	volatile long sequence_number;

	volatile long refcount;
} AVS_VideoFrameBuffer;

// AVS_VideoFrame is layed out identicly to IVideoFrame
// DO NOT USE THIS STRUCTURE DIRECTLY
typedef struct AVS_VideoFrame {
	volatile long refcount;
	AVS_VideoFrameBuffer * vfb;
	int offset, pitch, row_size, height, offsetU, offsetV, pitchUV;  // U&V offsets are from top of picture.
	int row_sizeUV, heightUV;
} AVS_VideoFrame;

// --------------------------------------------------------------------------

// AVS_VideoInfo is layed out identicly to VideoInfo
typedef struct AVS_VideoInfo {
	int width, height;    // width=0 means no video
	unsigned fps_numerator, fps_denominator;
	int num_frames;

	int pixel_type;

	int audio_samples_per_second;   // 0 means no audio
	int sample_type;
	INT64 num_audio_samples;
	int nchannels;

	// Imagetype properties

	int image_type;
} AVS_VideoInfo;

// --------------------------------------------------------------------------

AVSI_API(void, avs_bit_blt)(AVS_ScriptEnvironment *, AVSI_BYTE* dstp, int dst_pitch, const AVSI_BYTE* srcp, int src_pitch, int row_size, int height);
AVSI_API(const char *, avs_clip_get_error)(AVS_Clip *); // return 0 if no error
AVSI_API(AVS_ScriptEnvironment *, avs_create_script_environment)(int version);
AVSI_API(void, avs_delete_script_environment)(AVS_ScriptEnvironment *);
AVSI_API(int, avs_get_audio)(AVS_Clip *, void * buf,INT64 start, INT64 count);
AVSI_API(const char *, avs_get_error)(AVS_ScriptEnvironment *); // return 0 if no error
AVSI_API(AVS_VideoFrame *, avs_get_frame)(AVS_Clip *, int n);
AVSI_API(int, avs_get_version)(AVS_Clip *);
AVSI_API(const AVS_VideoInfo *, avs_get_video_info)(AVS_Clip *);
AVSI_API(AVS_Value, avs_invoke)(AVS_ScriptEnvironment *, const char * name, AVS_Value args, const char** arg_names);
AVSI_API(void, avs_release_clip)(AVS_Clip *);
AVSI_API(void, avs_release_value)(AVS_Value);
AVSI_API(void, avs_release_video_frame)(AVS_VideoFrame *);
AVSI_API(AVS_Clip *, avs_take_clip)(AVS_Value, AVS_ScriptEnvironment *);

AVSI_API(int, avs_bits_per_pixel)(const AVS_VideoInfo * p);
AVSI_API(int, avs_get_height_p)(const AVS_VideoFrame * p, int plane);
AVSI_API(int, avs_get_pitch_p)(const AVS_VideoFrame * p, int plane);
AVSI_API(const BYTE *, avs_get_read_ptr_p)(const AVS_VideoFrame * p, int plane);
AVSI_API(int, avs_get_row_size_p)(const AVS_VideoFrame * p, int plane);
AVSI_API(int, avs_is_planar_rgb)(const AVS_VideoInfo * p);
AVSI_API(int, avs_is_planar_rgba)(const AVS_VideoInfo * p);

inline int avs_defined(AVS_Value v) { return v.type != 'v'; }
inline int avs_is_clip(AVS_Value v) { return v.type == 'c'; }
inline int avs_is_bool(AVS_Value v) { return v.type == 'b'; }
inline int avs_is_int(AVS_Value v) { return v.type == 'i'; }
inline int avs_is_float(AVS_Value v) { return v.type == 'f' || v.type == 'i'; }
inline int avs_is_string(AVS_Value v) { return v.type == 's'; }
inline int avs_is_array(AVS_Value v) { return v.type == 'a'; }
inline int avs_is_error(AVS_Value v) { return v.type == 'e'; }

inline int avs_as_bool(AVS_Value v)
{
	return v.d.boolean;
}
inline int avs_as_int(AVS_Value v)
{
	return v.d.integer;
}
inline const char * avs_as_string(AVS_Value v)
{
	return avs_is_error(v) || avs_is_string(v) ? v.d.string : 0;
}
inline double avs_as_float(AVS_Value v)
{
	return avs_is_int(v) ? v.d.integer : v.d.floating_pt;
}
inline const char * avs_as_error(AVS_Value v)
{
	return avs_is_error(v) ? v.d.string : 0;
}

inline AVS_Value avs_new_value_bool(int v0)
{
	AVS_Value v = { 0 }; v.type = 'b'; v.d.boolean = v0 == 0 ? 0 : 1; return v;
}
inline AVS_Value avs_new_value_int(int v0)
{
	AVS_Value v = { 0 }; v.type = 'i'; v.d.integer = v0; return v;
}
inline AVS_Value avs_new_value_string(const char * v0)
{
	AVS_Value v = { 0 }; v.type = 's'; v.d.string = v0; return v;
}
inline AVS_Value avs_new_value_float(float v0)
{
	AVS_Value v = { 0 }; v.type = 'f'; v.d.floating_pt = v0; return v;
}
inline AVS_Value avs_new_value_error(const char * v0)
{
	AVS_Value v = { 0 }; v.type = 'e'; v.d.string = v0; return v;
}
