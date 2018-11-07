#include "stdafx.h"
#include "avsclipobj.h"
#include <INIReader.h>
#include "avsException.h"
#include "SharedMemManager.h"
#include "SharedMemHelper.h"
#include <sstream> 

using namespace std;

/*static*/std::string AvsClipObj::SectionSharedMem = "SharedMem";
/*static*/std::string AvsClipObj::Name_SharedMemName = "Name";

AvsClipObj::AvsClipObj()
	: state(State::Virgin),
	pSmMan(nullptr),
	curVideoFrameNo(0)
{
	this->videoFrame.clipObj = this;
}

void AvsClipObj::InitFromFile(const char* szFilename)
{
	this->ThrowIfStateIsNot(State::Virgin);
	INIReader reader(szFilename);
	int parseErr = reader.ParseError();
	if (parseErr != 0)
	{
		// TODO: provide error string
		throw AvsException(AvsException::Reason::ErrorReadingAvsFileIOError);
	}

	auto strSharedMemName = reader.Get(
		AvsClipObj::SectionSharedMem,
		AvsClipObj::Name_SharedMemName,
		"");

	if (strSharedMemName.empty())
	{
		// TODO: provide error string
		throw AvsException(AvsException::Reason::ErrorReadingAvsFileInvalidData);
	}

	this->sharedMemName = strSharedMemName;

	this->state = State::IniFileRead;
}

void AvsClipObj::InitializeSharedMem()
{
	this->ThrowIfStateIsNot(State::IniFileRead);

	//unique_ptr<SharedMemManager> upSmMan(new SharedMemManager(this->sharedMemName.c_str()));
	auto upSmMan = make_unique< SharedMemManager>(this->sharedMemName.c_str());
	bool b = upSmMan->Initialize();
	if (b != true)
	{
		throw AvsException(AvsException::Reason::ErrorInitalizingSharedMemory);
	}

	unique_ptr<SharedMemHelperReader> upSmHlp;
	try
	{
		upSmHlp = make_unique< SharedMemHelperReader>(upSmMan->GetSharedMemHdrPointer());
		//unique_ptr<SharedMemHelperReader> upSmHlp(new SharedMemHelperReader(upSmMan->GetSharedMemHdrPointer()));
	}
	catch (invalid_argument& e)
	{
		std::throw_with_nested(AvsException(AvsException::Reason::ErrorSharedMemoryInvalid));
	}

	b = upSmHlp->Probe();
	if (b == false)
	{
		throw AvsException(AvsException::Reason::ErrorProbingSharedMemory);
	}

	const auto sharedMemHdr = upSmMan->GetSharedMemHdrPointer();
	memset(&this->avsVideoInfo, 0, sizeof(this->avsVideoInfo));
	this->avsVideoInfo.width = sharedMemHdr->videoInfo.width;
	this->avsVideoInfo.height = sharedMemHdr->videoInfo.height;
	this->avsVideoInfo.fps_numerator = sharedMemHdr->videoInfo.fps_numerator;
	this->avsVideoInfo.fps_denominator = sharedMemHdr->videoInfo.fps_denominator;
	this->avsVideoInfo.pixel_type = sharedMemHdr->videoInfo.pixelType;// AVS_CS_BGR24;
	this->avsVideoInfo.audio_samples_per_second = 0;
	this->avsVideoInfo.num_frames = sharedMemHdr->videoInfo.numberOfFrames > 0 ?
		sharedMemHdr->videoInfo.numberOfFrames : (numeric_limits<uint32_t>::max)();

	this->pSmMan = upSmMan.release();
	this->pSmHelper = upSmHlp.release();
}

const char* AvsClipObj::GetLastError() const
{
	return this->lastError.empty() ? nullptr : this->lastError.c_str();
}

void AvsClipObj::ThrowIfStateIsNot(State state)
{
	if (this->state != state)
	{
		throw AvsException(AvsException::Reason::ErrorInvalidState);
	}
}

const AVS_VideoInfo* AvsClipObj::GetVideoInfo(int streamNo)
{
	if (streamNo != 0)
	{
		return nullptr;
	}

	return &this->avsVideoInfo;
}

AVSVideoFrameEx* AvsClipObj::GetVideoFrame(int n)
{
	// TODO: check state
	
	if (n != this->curVideoFrameNo)
	{
		stringstream ss;
		ss << "Current frame is " << n << ", and " << n << " was requested.";
		this->SetLastError(ss.str());
		return nullptr;
	}

	// wait until next frame is available
	FrameLockInfo frameLockInfo;
	bool b = this->pSmHelper->TryLockNextFrameWithTightLoopAndSlowLoop(frameLockInfo, 10, 30 * 1000);
	if (b == false)
	{
		stringstream ss;
		ss << "Timeout while waiting for next frame";
		this->SetLastError(ss.str());
		return nullptr;
	}

	this->videoFrame.frameLockInfo = frameLockInfo;

	return &this->videoFrame;
}

void AvsClipObj::ReleaseCurVideoFrame()
{
	this->pSmHelper->FreeLockedFrame(this->videoFrame.frameLockInfo);
	this->curVideoFrameNo++;
}

void AvsClipObj::SetLastError(const char* sz)
{
	this->lastError = sz;
}

const void* AvsClipObj::GetCurVideoFramePointer(const AVSVideoFrameEx* vf)
{
	bool b = this->CheckIfVideoFrameIsCurrentOne(vf);
	return this->pSmHelper->GetLockedFramePointer(vf->frameLockInfo);
}

int AvsClipObj::GetCurVideoFrameStride(const AVSVideoFrameEx* vf)
{
	bool b = this->CheckIfVideoFrameIsCurrentOne(vf);
	return (int)this->pSmHelper->GetVideoInfo().stride;
}

bool AvsClipObj::CheckIfVideoFrameIsCurrentOne(const AVSVideoFrameEx* vf)
{
	return true;
}

