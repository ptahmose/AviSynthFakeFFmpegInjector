#pragma once

#include <cstdint>

class HpTimer
{
private:
	static LONGLONG perfCounterFreq;

	LONGLONG startCounter;
public:
	HpTimer();

	void Start();

	std::uint32_t GetElapsedTime();

};