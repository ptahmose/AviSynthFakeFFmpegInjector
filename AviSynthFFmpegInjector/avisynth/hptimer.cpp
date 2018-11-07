#include "stdafx.h"
#include "hptimer.h"
#include <algorithm>
#include <limits>

using namespace std;

LONGLONG HpTimer::perfCounterFreq = []
{
	LARGE_INTEGER li;
	QueryPerformanceFrequency(&li);
	return li.QuadPart;
}();

HpTimer::HpTimer()
{
	this->Start();
}

void HpTimer::Start()
{
	LARGE_INTEGER li;
	QueryPerformanceCounter(&li);
	this->startCounter = li.QuadPart;
}

std::uint32_t HpTimer::GetElapsedTime()
{
	LARGE_INTEGER now;
	QueryPerformanceCounter(&now);

	auto diff = ((now.QuadPart - this->startCounter) * 1000) / HpTimer::perfCounterFreq;

	return (uint32_t)(std::min)(diff, (LONGLONG)(numeric_limits<uint32_t>::max)());
}
