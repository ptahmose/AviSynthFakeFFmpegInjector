#pragma once

#include <string>
#include "extFunc.h"
#include "avsvideoframe.h"

class SharedMemManager;
class SharedMemHelperReader;

class AvsClipObj
{
private:
	enum class State
	{
		Virgin,
		IniFileRead,
		SharedMemInitialized
	};

	State state;

private:
	AVS_VideoInfo avsVideoInfo;

private:
	int curVideoFrameNo;

	AVSVideoFrameEx videoFrame;
private:
	static std::string SectionSharedMem; 
	static std::string Name_SharedMemName;
	std::string sharedMemName;
	std::string lastError;
private:
	SharedMemManager* pSmMan;
	SharedMemHelperReader* pSmHelper;
public:
	AvsClipObj();

	void InitFromFile(const char* szFilename);

	void InitializeSharedMem();

	void ClearLastError();

	const AVS_VideoInfo* GetVideoInfo(int streamNo);

	AVSVideoFrameEx* GetVideoFrame(int n);

	const void* GetCurVideoFramePointer(const AVSVideoFrameEx* vf);
	int GetCurVideoFrameStride(const AVSVideoFrameEx* vf);
	void ReleaseCurVideoFrame();

	/// <summary>
	/// Gets the "last error string" if there is one. Otherwise, returns nullptr.
	/// The pointer returned is guaranteed to be valid until either another operation on the
	/// object is executed or the object is destroyed.
	/// This semantic is inherently dangerous, but this is inline with the AviSynth-API and its
	/// usage in ffmpeg.
	/// </summary>
	/// <returns>
	/// A null-terminated string if the is a last-error-condition; null otherwise.
	/// </returns>
	const char* GetLastError() const;

	void SetLastError(const char* sz);
	void SetLastError(const std::string& s) { this->SetLastError(s.c_str()); }
private:
	void ThrowIfStateIsNot(State state);
	bool CheckIfVideoFrameIsCurrentOne(const AVSVideoFrameEx* vf);
};
