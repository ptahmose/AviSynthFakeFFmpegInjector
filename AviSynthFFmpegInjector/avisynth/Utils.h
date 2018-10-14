#pragma once

#include <functional>

const int LogLevel_Trace = 1;

void Log(int level, std::function < std::string(void) > func);
