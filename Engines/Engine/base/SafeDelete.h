#pragma once

template<class T> inline void SafeDelete(T*& p) {
	delete p;
	p = nullptr;
}
