#pragma once
#include <string>

std::wstring ConvertString(const std::string& str);

std::string ConvertString(const std::wstring& str);

void Log(const std::string& message);
