#pragma once

#include <exception>
#include <string>

class AvsException : public std::exception
{
public:
	enum class Reason
	{
		ErrorInvalidState,
		ErrorReadingAvsFileIOError,
		ErrorReadingAvsFileInvalidData,

		/// <summary>
		/// An enum constant representing that there was an error opening the shared-memory.
		/// </summary>
		ErrorInitalizingSharedMemory,

		/// <summary>
		/// An enum constant representing the error that the shared memory contains invalid data.
		/// </summary>
		ErrorSharedMemoryInvalid,

		ErrorProbingSharedMemory
	};
private:
	Reason	reason;
	std::string text;
public:
	AvsException(Reason r) : AvsException(r, nullptr) {};
	AvsException(Reason r, const std::string& text) : AvsException(r, text.c_str()) {};
	AvsException(Reason r, const char* text);

	virtual const char* what() const throw()
	{
		return this->text.c_str();
	}

	std::string GetText() const;
private:
	static const char* ReasonAsText(Reason r);
};
