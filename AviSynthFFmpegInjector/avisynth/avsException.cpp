#include "stdafx.h"
#include "avsException.h"
#include <sstream> 

using namespace std;

AvsException::AvsException(Reason r, const char* text) : reason(r), text(text != nullptr ? text : "")
{
}

std::string AvsException::GetText() const
{
	stringstream ss;
	ss << AvsException::ReasonAsText(this->reason) << ": " << this->text;
	return ss.str();
}

/*static*/const char* AvsException::ReasonAsText(Reason r)
{
	switch (r)
	{
	case Reason::ErrorReadingAvsFileIOError:
		return "ErrorReadingAvsFileIOError";
	case Reason::ErrorReadingAvsFileInvalidData:
		return "ErrorReadingAvsFileInvalidData";
	case Reason::ErrorInvalidState:
		return "ErrorInvalidState";
	}

	return "<Unknown>";
}