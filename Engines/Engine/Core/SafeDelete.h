#pragma once

/// <summary>
/// deleteを安全に行うための関数
/// </summary>
/// <typeparam name="T"></typeparam>
/// <param name="p"></param>
template<class T> inline void SafeDelete(T*& p) {
	delete p;
	p = nullptr;
}
