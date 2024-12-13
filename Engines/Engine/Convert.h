#pragma once
#include <string>

//マルチバイト文字列からワイド文字列に変換
std::wstring ConvertString(const std::string& str);
//ワイド文字列からマルチバイト文字列に変換
std::string ConvertString(const std::wstring& str);
//ログ表示
void Log(const std::string& message);
