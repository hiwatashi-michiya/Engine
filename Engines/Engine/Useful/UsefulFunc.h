#pragma once
#include <string>
#include <vector>
#include <cmath>
#include <filesystem>
#include <windows.h>
#include "Vector4.h"
#include "Vector3.h"
#include <stdint.h>

///
/// 便利な関数を纏めていくヘッダー
///

//リソースファイルの検索
std::string SearchResourceFile(const std::string& name, const std::string& extension);

