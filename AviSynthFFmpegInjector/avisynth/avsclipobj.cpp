#include "stdafx.h"
#include "avsclipobj.h"
#include <INIReader.h>
#include "avsException.h"
#include "SharedMemManager.h"
#include "SharedMemHelper.h"

using namespace std;

/*static*/std::string AvsClipObj::SectionSharedMem = "SharedMem";
/*static*/std::string AvsClipObj::Name_SharedMemName = "Name";

AvsClipObj::AvsClipObj()
	: state(State::Virgin),
	  pSmMan(nullptr)
{
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
	
	unique_ptr<SharedMemManager> upSmMan(new SharedMemManager(this->sharedMemName.c_str()));

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