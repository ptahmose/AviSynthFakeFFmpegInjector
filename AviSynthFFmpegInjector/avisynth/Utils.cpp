#include "stdafx.h"
#include "Utils.h"

void Log(int level, std::function < std::string(void) > func)
{
	auto str = func();
	OutputDebugStringA(str.c_str());
}