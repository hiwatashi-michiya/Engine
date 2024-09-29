#pragma once
#include <string>
#include <vector>
#include <cmath>
#include <filesystem>
#include <windows.h>
#include "Vector4.h"
#include "Vector3.h"
#include <stdint.h>

std::string SearchResourceFile(const std::string& name, const std::string& extension);

//数字から色を作る
Vector4 CreateColor(int32_t num);

Vector3 CreateVector3Color(int32_t num);
