#include "Logger.h"
#include "MyWindow.h"

void Log(const std::string& message)
{
	OutputDebugStringA(message.c_str());
}

void Log(const std::wstring& message)
{
	OutputDebugStringW(message.c_str());
}
