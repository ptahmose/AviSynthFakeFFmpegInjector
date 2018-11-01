#pragma once

#include <string>

class SharedMemManager;

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
	static std::string SectionSharedMem; 
	static std::string Name_SharedMemName;
	std::string sharedMemName;
	std::string lastError;
private:
	SharedMemManager* pSmMan;
public:
	AvsClipObj();

	void InitFromFile(const char* szFilename);

	void InitializeSharedMem();

	void ClearLastError();

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

private:
	void ThrowIfStateIsNot(State state);
};
